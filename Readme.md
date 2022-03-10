# Usage:

int main(void)
{
  SHT31_Config(SHT31_ADDRESS_A, &hi2c1);
  HAL_Delay(2000);
  while (1)
  {
	  if(SHT31_GetData(SHT31_Periodic, SHT31_Medium, SHT31_NON_Stretch, SHT31_1) == SHT31_OK) {
		  temperature = SHT31_GetTemperature();
		  humidity = SHT31_GetHumidity();
	  }

	  HAL_Delay(200);
  }
}