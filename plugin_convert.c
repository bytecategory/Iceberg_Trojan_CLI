#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>
#include <openssl/des.h>
int main(int argc,char **argv){
	unsigned char *g;
	unsigned char *r;
	unsigned char *p;
	unsigned char *e;
	unsigned char *l;
	unsigned char s[]="iceberg_plu_cut_text_@";
	DES_key_schedule c;
	FILE *f;
	size_t n;
	uLong rl,pl,el;
	if (argc<2) {
		return 1;
	}
	f=fopen(argv[1],"rb");
	if (!f) {
		return 1;
	}
	fseek(f,0,SEEK_END);
	n=ftell(f);
	rewind(f);
	g=malloc(n);
	fread(g,1,n,f);
	fclose(f);
	r=malloc(1024*1024*10);
	rl=1024*1024*10;
	if (uncompress(r,&rl,g+8,n-8)!=Z_OK) {
                return 1;
	}
	p=memmem(r,rl,s,22);
	if (!p) { 
		return 1; 
	}
	pl=p-(r+8);
	e=malloc(1024*1024*10);
	el=1024*1024*10;
	if (uncompress(e,&el,r+8,pl) != Z_OK) {
		return 1;
	}
	DES_cblock k={0};
	DES_set_key_unchecked(&k,&c);
	for(size_t i=0;i<el;i+=8) {
		DES_ecb_encrypt((DES_cblock *)(e+i),(DES_cblock *)(e+i),&c,DES_DECRYPT);
	}
	l=e+4;
	char o[512];
        strncpy(o,argv[1],sizeof(o));
        o[sizeof(o)-1]=0;
        char *t=strrchr(o,'.');
        if(t)*t=0;
        strcat(o,".exe");
	f=fopen(o,"wb");
	fwrite(l,1,el-4,f);
	fclose(f);
	return 0;
}

