
#include "IS25LP128F.h"
#include "string.h"

uint8_t IS25LP128F_detect_status = 0u;

void IS25LP128F_init(void){
    IS25LP128F_detect_status = 0u;

    spi_init();
	PIN_FL_RESET_GPIO->PCOR	= PIN_FL_RESET;
	PIN_FL_RESET_GPIO->PSOR	= PIN_FL_RESET;
}

uint8_t IS25LP128F_detect(void){
    if((IS25LP128F_sfdp(0x00u) == 0x53u)
    && (IS25LP128F_sfdp(0x01u) == 0x46u)
    && (IS25LP128F_sfdp(0x02u) == 0x44u)
    && (IS25LP128F_sfdp(0x03u) == 0x50u)){
        IS25LP128F_detect_status = 1u;
    }
    else{
        IS25LP128F_detect_status = 0u;
    }

    return IS25LP128F_detect_status;
}

uint8_t IS25LP128F_is_detected(void)
{
    return IS25LP128F_detect_status;
}

void IS25LP128F_read(uint8_t *buf, uint32_t addr, uint32_t len){
	
	
	uint8_t *p = (uint8_t *)buf;

	IS25LP128F_is_busy();		//wait while busy
	
	do {
		uint32_t rdlen = len;
		uint8_t reg = IS25LP128F_bank_reg();
		spi_cs_low();
		if ( reg & IS25LP128F_EXTADD_MASK) {
			spi_shift(0x03);
			spi_shift_16(addr >> 16);
			spi_shift_16(addr);
		} else {
			spi_shift_16(0x0300 | ((addr >> 16) & 255));
			spi_shift_16(addr);
		}
		for(int i = 0; i<rdlen; i++){
			p[i] = spi_shift(0xFF);
		}
		spi_cs_high();
		p += rdlen;
		addr += rdlen;
		len -= rdlen;
	} while (len > 0);
}

void IS25LP128F_program(uint8_t *buf, uint32_t addr, uint32_t len){
	
	const uint8_t *p = (const uint8_t *)buf;
	uint32_t max, pagelen;

	 //Serial.printf("WR: addr %08X, len %d\n", addr, len);
	do {
		IS25LP128F_is_busy();
		
		IS25LP128F_write_enable();

		max = 256 - (addr & 0xFF);
		pagelen = (len <= max) ? len : max;
		
		uint8_t reg = IS25LP128F_bank_reg();
		spi_cs_low();
		if ( reg & IS25LP128F_EXTADD_MASK) {
			spi_shift(0x02); // program page command
			spi_shift_16(addr >> 16);
			spi_shift_16(addr);
		} else {
			spi_shift_16(0x0200 | ((addr >> 16) & 255));
			spi_shift_16(addr);
		}
		addr += pagelen;
		len -= pagelen;
		do {
			spi_shift(*p++);
		} while (--pagelen > 0);
		spi_cs_high();
	} while (len > 0);
}

uint8_t IS25LP128F_sfdp(uint8_t addr){
	uint8_t data;
	spi_cs_low();
	spi_shift(0x5a);
	spi_shift(0x00);
	spi_shift(0x00);
	spi_shift(addr);
	spi_shift(0xFF);	//dummy
	data = spi_shift(0xFF);
	spi_cs_high();
	return data;
}

void IS25LP128F_is_busy(void){
	uint8_t status;
	while(1){
		status = IS25LP128F_status();
		if(!(status & IS25LP128F_WIP_MASK)){
			break;
		}
	}
}

uint8_t IS25LP128F_status(void){
	uint8_t reg;
	spi_cs_low();
	spi_shift(0x05);
	reg = spi_shift(0xFF);
	spi_cs_high();
	return reg;
}

uint8_t IS25LP128F_bank_reg(void){
	uint8_t reg;
	spi_cs_low();
	spi_shift(0x16);
	reg = spi_shift(0xFF);
	spi_cs_high();
	return reg;
}


void IS25LP128F_delete_chip(void){
	IS25LP128F_is_busy();
	
	IS25LP128F_write_enable();
	
	//chip erase  C7h/60h
	spi_cs_low();
	spi_shift(0xC7);
	spi_cs_high();
}

void IS25LP128F_delete_block(uint32_t blk_add){

    IS25LP128F_is_busy();       //wait while busy
	
	IS25LP128F_write_enable();
	
	spi_cs_low();
	//block erase
	//D8h=64k 52h=32k
	if (IS25LP128F_bank_reg() & IS25LP128F_EXTADD_MASK) {
		spi_shift(0x52);
		spi_shift_16(blk_add >> 16);
		spi_shift_16(blk_add);
	} else {
		spi_shift_16(0x5200 | ((blk_add >> 16) & 255));
		spi_shift_16(blk_add);
	}
	spi_cs_high();
}

void IS25LP128F_delete_sector(uint32_t sec_add){
	
    IS25LP128F_is_busy();       //wait while busy
	
	IS25LP128F_write_enable();

	spi_cs_low();
	//sector erase D7h/20h
	if (IS25LP128F_bank_reg() & IS25LP128F_EXTADD_MASK) {
		spi_shift(0x20);
		spi_shift_16(sec_add >> 16);
		spi_shift_16(sec_add);
	} else {
		spi_shift_16(0x2000 | ((sec_add >> 16) & 255));
		spi_shift_16(sec_add);
	}
	spi_cs_high();
	
}

void IS25LP128F_write_enable(void){
	spi_cs_low();
	// write enable command
	spi_shift(0x06);
	spi_cs_high();
	
	uint8_t status;
	while(1){
		status = IS25LP128F_status();
		if(status & IS25LP128F_WEL_MASK){
			break;
		}
	}
}

void IS25LP128F_suspend(void){
	spi_cs_low();
	//75h/B0h
	spi_write(0x75);
	spi_cs_high();
}

void IS25LP128F_resume(void){
	spi_cs_low();
	//7Ah/30h
	spi_write(0x7A);
	spi_cs_high();
}

uint8_t IS25LP128F_func_reg(void){
	uint8_t reg;
	spi_cs_low();
	spi_shift(0x48);
	reg = spi_shift(0xFF);
	spi_cs_high();
	return reg;
}

uint8_t IS25LP128F_extread_reg(void){
    uint8_t reg;
    spi_cs_low();
    spi_shift(0x81); //RDERP
    reg = spi_shift(0xFF);
    spi_cs_high();
    return reg;
}



