/*
    Driver source for testing the bilby implemenation
*/
#include "../include/bilby.h"
#include <stdio.h>

int main(void)
{
  /* Develop bilby */
  struct bilby_instance * p_instance = bilby_create_instance();
  printf("\nInstance : %s", p_instance == NULL ? "NULL" : "OK");
  bilby_destroy_instance(&p_instance);
  printf("\nDestroyal: %s", p_instance == NULL ? "OK" : "Not destroyed");
  printf("\n\n");


  /* Return to OS successfully */
  return 0;
}