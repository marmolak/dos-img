#include <cstdio>
#include <cstdlib>
#include <cstdlib>
#include <dos.h>
#include <conio.h>
#include <fstream>
#include <string>
#include <istream>
#include <cstring>

/* VGA Palette access port addresses */
#define PALETTE_MASK  0x3C6
#define PALETTE_READ  0x3C7
#define PALETTE_WRITE 0x3C8
#define PALETTE_DATA  0x3C9

void switch_mode(char mode)
{
    union REGS i_regs;
    union REGS o_regs;
    struct SREGS s_regs;
    i_regs.h.ah = 0x00;
    i_regs.h.al = mode;
    //i_regs.x.ax = mode;
    int86x(0x10, &i_regs, &o_regs, &s_regs);
}

void switch_graphics()
{
	// 0x13 -> 320x200 256
	switch_mode(0x13);
}


void switch_text()
{
	switch_mode(0x03);
}

struct RGB_Color {
      unsigned char red;
      unsigned char green;
      unsigned char blue;
};

RGB_Color old_pallete[256];
void save_pallete()
{
	_disable();

	outp(PALETTE_MASK, 0xFF);

	for (unsigned short i = 0; i < 256; ++i)
	{
	      /* Send color palette register index to write to */
	      outp(PALETTE_READ, i);

	      /* Write the color palette entry */
	      old_pallete[i].red 	= inp(PALETTE_DATA); 
	      old_pallete[i].green 	= inp(PALETTE_DATA);
	      old_pallete[i].blue 	= inp(PALETTE_DATA); 
	 }

	_enable();
}

void fill_2()
{
	_disable();

	outp(PALETTE_MASK, 0xFF);

	for (unsigned short i = 0; i < 256; ++i)
	{
	      /* Send color palette register index to write to */
	      outp(PALETTE_WRITE, i);

	      /* Write the color palette entry */
	      outp(PALETTE_DATA, old_pallete[i].red); 
	      outp(PALETTE_DATA, old_pallete[i].green);
	      outp(PALETTE_DATA, old_pallete[i].blue); 
	 }

	_enable();

}

void fill_pallete()
{
	_disable();

	outp(PALETTE_MASK, 0xFF);

	for (unsigned short i = 0; i < 256; ++i)
	{
	      /* Send color palette register index to write to */
	      outp(PALETTE_WRITE, i);

	      /* Write the color palette entry */
	      outp(PALETTE_DATA, 0x00); 
	      outp(PALETTE_DATA, 0x00);
	      outp(PALETTE_DATA, 0x00); 
	 }

	_enable();
}


char buff[320];


int main()
{
	std::cout << "Please wait..." << std::endl;

	unsigned char *double_buffer = (unsigned char *) std::malloc(320);
	std::ifstream fout;
	fout.open("image.raw");

	switch_graphics();
	save_pallete();
	fill_pallete();

	unsigned char far *p = (unsigned char far *)0xA0000000L;

	do
	{
		fout.read(double_buffer, 320);
		if (fout.eof()) {
			break;
		}
		_fmemcpy(p, double_buffer, (size_t) 320);
		p += fout.gcount();
	} while (true);
	fout.close();

	fill_2();

	std::getchar();
	switch_text();
    	return EXIT_SUCCESS;
}

