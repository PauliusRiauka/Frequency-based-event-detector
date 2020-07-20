#include <Arduino.h>
#include "CComplex.h"
#include <math.h>

//variables for ADC conversion and recording
const uint16_t nSamples = 2048; //number of samples to work with
uint16_t NewADCVal; //new ADC value
uint16_t storage[nSamples]; //array to store ADC values for conversion
uint16_t n= 0; //variable to track how many samples were recorded
uint16_t mid=2048; //value around which the signal oscillates 
uint16_t Athreshold = 700; // Amplitude threshold to start ADC recording
static bool record=0; //Recording on/off
static bool FFTflag=0; //Perform FFT/ Wait for next sample

//Variables for iterative FFT and output conversion
CComplex A[nSamples]; // array to output FFT values 
int log2n=11; //log2(2048) to start the butterfly FFT bottom-up 
double Output[nSamples/2]; //array to store processed FFT values
double Havg=0; //average value of the top 3/4 of the processed FFT
double Lavg=0; //average value of the bottom 1/4 of the processed FFT
double Average; //Difference between the averages
uint16_t Fthreshold = 50; //Frequency threshold to consider the input a clap

uint8_t Sequence=0;
uint8_t Counter=0;
uint8_t twoclaps=0;
uint8_t threeclaps=0;
uint32_t start;
uint32_t dur;

//allign the variables of the storage array to perform the butterfly FFT bottom-up
unsigned int bitReversal(unsigned int i, int log2n) 
{ 
	int v = 0; 
	for (uint16_t d = 0; d < log2n; d++) 
	{ 
		v <<= 1; 
		v |= (i & 1); 
		i >>= 1; 
	} 
	return v; 
} 

//Iterative FFT function
void fft(uint16_t a[], CComplex A[], int log2n, uint16_t n) 
{ 
  //Perform bit reversal
	for (int i = 0; i < n; ++i) { 
		int rev = bitReversal(i, log2n); 
		A[i] = a[rev]; 
	} 

	CComplex J(0, 1); 
	for (int s = 1; s <= log2n; ++s) 
  { 
		int m = 1 << s; // 2 power s 
		int m2 = m >> 1; // m2 = m/2 
		CComplex w(1, 0); 
		CComplex wm = (J * (PI / m2)).c_exp(); 
		for (int j = 0; j < m2; ++j) { 
			for (int k = j; k < n; k += m) { 
				CComplex t = w * A[k + m2]; 
				CComplex u = A[k]; 
				A[k] = u + t; 
				A[k + m2] = u - t; 
			} 
			w *= wm; 
		} 
	} 
} 

void setup()
{
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pmc_enable_periph_clk(ID_ADC); // To use peripheral, we must enable clock distributon to it
  adc_init(ADC, SystemCoreClock, 0xe3d00, ADC_STARTUP_FAST); // initialize, set maximum posibble speed
  adc_disable_interrupt(ADC, 0xFFFFFFFF);
  adc_set_resolution(ADC, ADC_12_BITS);
  adc_configure_power_save(ADC, 0, 0); // Disable sleep mode
  adc_configure_timing(ADC, 0, ADC_SETTLING_TIME_3, 1); // Set timings - standard values
  adc_set_bias_current(ADC, 1); // Bias current - maximum performance over current consumption
  adc_stop_sequencer(ADC); // not using it
  adc_disable_tag(ADC); // it has to do with sequencer, not using it
  adc_disable_ts(ADC); // disable temperature sensor
  adc_disable_channel_differential_input(ADC, ADC_CHANNEL_7);
  adc_configure_trigger(ADC, ADC_TRIG_SW, 1); // triggering from software, freerunning mode
  adc_disable_all_channel(ADC);
  adc_enable_channel(ADC, ADC_CHANNEL_3); // Enable ADC channel on pin A4
  adc_start(ADC);
 
  /* turn on the timer clock in the power management controller */
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(ID_TC4);

  /* we want wavesel 01 with RC */
  TC_Configure(/* clock */TC1,/* channel */1, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK2);
  TC_SetRC(TC1, 1, 238); // 238 sets <> 44.1 Khz interrupt rate frequence echantillonage
  TC_Start(TC1, 1);

  // enable timer interrupts on the timer
  TC1->TC_CHANNEL[1].TC_IER = TC_IER_CPCS;
  TC1->TC_CHANNEL[1].TC_IDR = ~TC_IER_CPCS;

  /* Enable the interrupt in the nested vector interrupt controller */
  /* TC4_IRQn where 4 is the timer number * timer channels (3) + the channel number (=(1*3)+1) for timer1 channel1 */
  NVIC_EnableIRQ(TC4_IRQn);
  Serial.begin(115200);
}

void loop()
{
  if(FFTflag==1) // if storage is filled
  { 
    PIO_Set(PIOB,PIO_PB27B_TIOB0);
    NVIC_DisableIRQ(TC4_IRQn); //disable timer 4 interrupts
    for (int i = 0; i < nSamples; i++) //Clear FFT raw output array
    {
      float re = 0.0;
      float im = 0.0;
      A[i].set(re, im);
    }
    Havg=0;
    Lavg=0;
    fft(storage, A, log2n, nSamples); //perform FFT
    for (int h = 0; h < nSamples/2; h++) 
      {
        Output[h]=20*log((A[h].modulus())); //condition the raw FFT output
        if(h<204) //Find averages
        {
          Lavg+=Output[h];
        }else
        {
          Havg+=Output[h]; 
        }
      }       
    Lavg/=204;
    Havg/=820;
    Average=Lavg-Havg;
    if (Average > Fthreshold) //Clap?
    {
      Serial.println("CLAP!");
      Sequence++;
      if (Sequence==1)
        start = micros();
    }
    if (Counter==0)
    {
      digitalWrite(13, HIGH);
      Counter=1;
      if (Sequence==2)
      {
          dur = micros() - start;
          Sequence=0;
      }
    }
    if(Counter==1)
    {
      digitalWrite(13, LOW);
      Counter=0;
      if(Sequence==1)
      {
        dur = micros() - start;
        if (dur>1000000)
          Sequence=0;
      }
      if (Sequence==2)
      {
        dur = micros() - start;
        if (dur>1000000)
        {
         if (twoclaps == 0)  
         {
           digitalWrite(12, HIGH);
           twoclaps=1;
         }
         else
         {
           digitalWrite(12, LOW);
           twoclaps=0;
         }
         Sequence=0;
      }
      if (Sequence==3)
      {
        dur = micros() - start;
        if (dur<1000000)
        {
          if (threeclaps == 0)  
             {
                digitalWrite(11, HIGH);
                threeclaps=1;
             }
          else
             {
                digitalWrite(11, LOW);
                threeclaps=0;
             }
        }
            Sequence=0;
      }
      else
      {
          Sequence=0;
      }
    }
           
    }
    Serial.println(Average);
    FFTflag=0; //Clear "Storage full" flag
    PIO_Clear(PIOB,PIO_PB27B_TIOB0);
    NVIC_EnableIRQ(TC4_IRQn); //enable timer 4 interrupts
  }
}


//ISR
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TC4_Handler()
{
  TC_GetStatus(TC1, 1); //clear status
  NewADCVal = adc_get_latest_value(ADC); // Read ADC
  if (record == 1){ //if recording
    storage[n]=NewADCVal;
    n++;
    if (n>nSamples-1){ //if storage full, stop recording, enable FFT
      record=0;
      FFTflag=1;   
      n=0;
    }
  }
  else{
    if (NewADCVal < mid-Athreshold || NewADCVal > mid+Athreshold){ //if threshold was reached
      record=1; //start recording
      storage[n]=NewADCVal;
      n++; 
    }
  }
}