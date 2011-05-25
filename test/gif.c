#include<stdio.h>
/*
				REQ	LAB
Application Extension       Opt. (*)   0xFF (255) 
Comment Extension           Opt. (*)   0xFE (254)
Global Color Table          Opt. (1)   none      
Graphic Control Extension   Opt. (*)   0xF9 (249)
Header                      Req. (1)   none      
Image Descriptor            Opt. (*)   0x2C (044)
Local Color Table           Opt. (*)   none      
Logical Screen Descriptor   Req. (1)   none      
Plain Text Extension        Opt. (*)   0x01 (001)
Trailer                     Req. (1)   0x3B (059)

Unlabeled Blocks
Header                      Req. (1)   none    
Logical Screen Descriptor   Req. (1)   none   
Global Color Table          Opt. (1)   none  
Local Color Table           Opt. (*)   none 

Graphic-Rendering Blocks
Plain Text Extension        Opt. (*)   0x01 (001) 
Image Descriptor            Opt. (*)   0x2C (044) 

Control Blocks
Graphic Control Extension   Opt. (*)   0xF9 (249)

Special Purpose Blocks
Trailer                     Req. (1)   0x3B (059) 
Comment Extension           Opt. (*)   0xFE (254)
Application Extension       Opt. (*)   0xFF (255)
*/
#define GIF_VERSION	"GIF89a"
#define GIF_APPLICATION_EXTENSION	0xFF
#define GIF_COMMENT_EXTENSION		0xFE
#define GIF_CONTROL_EXTENSION		0xF9
#define GIF_IMAGE_DESCRIPTOR		0x2C
#define GIF_TRAILER	0x3B
#define GIF_EXTENSION	0x21
#define GIF_TERMINATOR	0x00
#define GIF_GLOBALCOLORTABLE	0x80

struct LZWblocks
{	unsigned char size;
	unsigned char*data;
};

struct datatriplet
{	unsigned char r;
	unsigned char g;
	unsigned char b;
};

struct triplet
{	unsigned short size;
	struct datatriplet*data;
};

struct gifimage
{	unsigned char	dblock;
	unsigned char	*ddata;	
	unsigned short	lwidth;	//logical screen width
	unsigned short	lheight; //logical screen height
	unsigned char	lpacketfield;
	unsigned char	lbackgroundcolorindex;
	unsigned char	lpixelaspectratio;
	struct triplet	gcolortable;
	struct triplet	lcolortable;
	unsigned char	iseperator;
	unsigned short	ileftposition;
	unsigned short	itopposition;
	unsigned short	iwidth;
	unsigned short	iheight;
	unsigned char	ipacketfield;
	unsigned char	clabel;
	struct triplet	cdata;
	struct triplet	idata;
	unsigned char	cterm;
	struct triplet	*image;	
	struct LZWblocks *lzw;
};

//This should bitpack correctly
unsigned char*short2char(short*toconv,int count)
{	unsigned char*ret,*p;
	int i=0;
	p=ret=(unsigned char*)malloc(count*2);	//won't need this much but . . .
	for(;i<count;i++)
	{	*ret=(unsigned char)(toconv[i]&0x00ff);
		ret++;
		*ret=(unsigned char)((toconv[i]&0x0f00)>>8);
		i++;
		*ret|=(unsigned char)((toconv[i]&0x000f)<<4);
		ret++;
		*ret=(unsigned char)((toconv[i]&0x0ff0)>>4);
	}
	return(ret);
}

void print_lzw(struct LZWblocks*toprint)
{	unsigned short n;
	printf("%c",toprint->size);
	printf("%s",toprint->data);
}

lzw_compress()
{
/*     STRING = get input character                                               
     WHILE there are still input characters DO                                  
         CHARACTER = get input character                                        
         IF STRING+CHARACTER is in the string table then                        
             STRING = STRING+character                                          
         ELSE                                                                   
             output the code for STRING                                         
             add STRING+CHARACTER to the string table                           
             STRING = CHARACTER                                                 
         END of IF                                                              
     END of WHILE                                                               
     output the code for STRING    
*/
}
void create_gif(struct gifimage*todo,unsigned short height,unsigned short width)
{	unsigned char n;
	todo->lwidth=width;
	todo->lheight=height;
	todo->lpacketfield=0x77;
	todo->iwidth=width;
	todo->iheight=height;
	todo->ileftposition=0;
	todo->itopposition=0;
	todo->ipacketfield=0x87;
	todo->image=(struct triplet*)malloc(4*height);
	for(;height;height--)
	{	todo->image[height].data=(struct datatriplet*)malloc(sizeof(struct datatriplet)*width);
		bzero(todo->image[height].data,3*width);
		todo->image[height].size=width;
	}
//Default table
	todo->lcolortable.data=(struct datatriplet*)malloc(sizeof(struct datatriplet)*256);
	for(n=0;n<255;n++)
	{	todo->lcolortable.data[n].r=n;
		todo->lcolortable.data[n].g=n;
		todo->lcolortable.data[n].b=n;
	}
	todo->lcolortable.size=255;
}

void print_triplet(struct triplet*toprint)
{	static unsigned short n;
	for(n=0;n<toprint->size;n++)
	{	printf("%c%c%c",
			toprint->data[n].r,
			toprint->data[n].g,
			toprint->data[n].b);
	}
}	

unsigned char*short2bin(unsigned short in)
{	static unsigned char out[2];
	out[0]=in>>8;
	out[1]=in;
	return(out);
}

void produce_gif(struct gifimage*todo) 
{	unsigned short n;	
	unsigned char depth=0x08;
	unsigned char clcode=depth*2;
	printf("%s",GIF_VERSION);
//Logical screen descriptor
	printf("%s%s%c%c%c",
		short2bin(todo->lwidth),
		short2bin(todo->lheight),
		todo->lpacketfield,
		todo->lbackgroundcolorindex,
		todo->lpixelaspectratio);
//Global color table
//Image descriptor
	printf("%c%s%s%s%s%c",
		GIF_IMAGE_DESCRIPTOR,
		short2bin(todo->ileftposition),
		short2bin(todo->itopposition),
		short2bin(todo->iwidth),
		short2bin(todo->iheight),
		todo->ipacketfield);
//Local color table	(O)
	print_triplet(&todo->lcolortable);
//Table based image data
	printf("%c",depth);//code size
	for(n=0;n<todo->lheight;n++)	
	{	print_triplet(&todo->image[n]);
	}
	printf("%c",clcode+1);//end of data

//Graphic control extension
//Comment extension
//Plain text extension
//Application extension
//Trailer	
	printf("%c",GIF_TRAILER);
}

//primitives

void fill(int x1,int y1)
{
	return;
}

void line(int x1,int y1,int x2,int y2)
{
	return;
}

void circle(int x1, int y1, int radius)
{
	return;
}

int main()
{	struct gifimage todo;
	create_gif(&todo,100,100);
	produce_gif(&todo);	
}
