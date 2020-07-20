//
//    FILE: array.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.00
//    DATE: 2015-10-18
//
// PUPROSE: example CComplex array
//

#include "CComplex.h"

CComplex samples[100];

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.println("\n  CComplex numbers array example");
  Serial.println("  Library version: ");
  Serial.println(CComplex_LIB_VERSION);

  Serial.println("\n1. Read into array");
  for (int i = 0; i < 100; i++)
  {
    float re = analogRead(A0) / 1023.0;
    float im = analogRead(A1) / 1023.0;
    samples[i].set(re, im);
  }

  Serial.println("\n2. Print array");
  for (int i = 0; i < 100; i++)
  {
    if (i % 5 == 0) Serial.println();
    Serial.print(samples[i]);
    Serial.print("\t");
  }
  Serial.println();

  Serial.println("\n3. Sum array");
  CComplex sum;
  for (int i = 0; i < 100; i++)
  {
    sum += samples[i];
  }
  Serial.println(sum);

  Serial.println("\n  done");
}

void loop()
{
}
//
// END OF FILE
//
