#ifndef spi_H
#define spi_H

void initSPI1();

void setVoltage(char channel, char voltage);

char SPI1_IO(char write);

#endif