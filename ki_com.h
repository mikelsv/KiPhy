#include <new>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#else
	#define memcpy memmove

	typedef unsigned char       BYTE;
	typedef unsigned short      WORD;
	typedef unsigned long       DWORD;	
	typedef DWORD   COLORREF;

	#ifndef RGB
		#define RGB(r,g,b)          ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
	#endif

	typedef unsigned long ULONG_PTR;
	typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;
	#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))

	#define GetRValue(rgb)      (LOBYTE(rgb))
	#define GetGValue(rgb)      (LOBYTE(((WORD)(rgb)) >> 8))
	#define GetBValue(rgb)      (LOBYTE((rgb)>>16))
#endif

#define MODUNICODE(x) (x) //<< normal

void globalerror(char *s){
	printf("%s", s);
}

#ifndef max
	#define max(a,b)    (((a) > (b)) ? (a) : (b))
	#define min(a,b)    (((a) < (b)) ? (a) : (b))
#endif

#ifndef PI
	#define PI          3.1415926535897932  /* pi */
#endif


// Lock
#include "crossplatform/lock.h"
#include "crossplatform/lock.cpp"

// OMatrixTemplates //

// Add to end (_p, _n);
#define OMatrixTemplateAdd(_a, _e, el)				\
	if(!_a){ _a=el; _e=el; el->_p=0; el->_n=0; }		\
	else {	el->_n=0; el->_p=_e; _e->_n=el; _e=el; }

// Add to end (only _n);
#define OMatrixTemplateAddO(_a, _e, el)				\
	if(!_a){ _a=el; _e=el; el->_n=0; }				\
	else {	el->_n=0; _e->_n=el; _e=el; }

// Add to end (Set _p, _n name);
#define OMatrixTemplateAddF(_a, _e, el, _p, _n)			\
	if(!_a){ _a=el; _e=el; el->_p=0; el->_n=0; }			\
	else {	el->_n=0; el->_p=_e; _e->_n=el; _e=el; }

// Add to before p;
#define OMatrixTemplateAddP(_a, _e, p, el)				\
	if(!_a){ _a=el; _e=el; el->_p=0; el->_n=0; }			\
	else if(!p){										\
		el->_p=0; el->_n=_a; _a->_p=el; _a=el;			\
	} else {											\
		el->_p=p; el->_n=p->_n;							\
		p->_n=el; if(el->_n) el->_n->_p=el; else _e=el;	\
	}

// Del
#define OMatrixTemplateDel(_a, _e, el)								\
	if(el->_n) el->_n->_p=el->_p; else if(el==_e) _e=el->_p;		\
	if(el->_p) el->_p->_n=el->_n; else if(el==_a) _a=el->_n;

#define OMatrixTemplateDelF(_a, _e, el, _p, _n)						\
	if(el->_n) el->_n->_p=el->_p; else if(el==_e) _e=el->_p;		\
	if(el->_p) el->_p->_n=el->_n; else if(el==_a) _a=el->_n;


#include "list/AList.h"
#include "list/OList.h"



class KiMouse{
public:
	// Left, center, right down
	int ld, cd, rd;
	// x, y, last
	int x, y, lx, ly;

	KiMouse(){
		ld = 0;
		cd = 0;
		rd = 0;
	}

} mouse;


void glut_mouse(int button, int state, int x, int y){
	if(button == GLUT_LEFT_BUTTON){
		if(state == GLUT_DOWN)
			mouse.ld = 1;
		else
			mouse.ld = 0;

		mouse.lx = mouse.x;
		mouse.ly = mouse.y;

		mouse.x = x;
		mouse.y = height - y;
	}
}

void glut_mouse_move(int x, int y){
		mouse.x = x;
		mouse.y = height - y;
}


// https://github.com/mikelsv/msvcore2 //
///// MRGB - My Red Green Blue ///////////////////////////////////////////////////////////
struct MRGB{
	BYTE	blue;//red;
	BYTE	green;
	BYTE	red;//blue;	

	MRGB(){ }
	MRGB(BYTE r, BYTE g, BYTE b){ red=r; green=g; blue=b; }
	MRGB(const int color){ red=GetRValue(color); green=GetGValue(color); blue=GetBValue(color); }
	//MRGB(const unsigned short cl){ unsigned int c=RGB16C(cl); red=GetRValue(c); green=GetGValue(c); blue=GetBValue(c); }
	MRGB(const char* cl){ set(cl); return ; }
	//MRGB(const VString cl){ set(cl); return ; }
//	MRGB(const MRGB cl){ red=cl.red; green=cl.green; blue=cl.blue; }

	inline void set(BYTE r, BYTE g, BYTE b){ red=r; green=g; blue=b; }
	
	inline void set(const char *ln){ set((unsigned char*)ln); return ; }
	//inline void set(const VString cl){
	//	if (cl.size()<6) { blue=0; green=0; red=0; return ;}
	//	set(cl.uchar()); return;}

	unsigned char upd(const unsigned char ch){ return ((ch>=97 && ch<=122 ||  ch>=224) ? ch-32 : ch); }

	inline void set(unsigned char *ln){ if(!ln){ red=0; green=0; blue=0; return ; } char c;
		c=upd(*ln); ln++; red=((c>47 && c<58) ? c-48 : c-55)*16;
		c=upd(*ln); ln++; red+=((c>47 && c<58) ? c-48 : c-55)%16;
		c=upd(*ln); ln++; green=((c>47 && c<58) ? c-48 : c-55)*16;
		c=upd(*ln); ln++; green+=((c>47 && c<58) ? c-48 : c-55)%16;
		c=upd(*ln); ln++; blue=((c>47 && c<58) ? c-48 : c-55)*16;
		c=upd(*ln); ln++; blue+=((c>47 && c<58) ? c-48 : c-55)%16;
	return ;}
	
	void operator=(const int color){
		red=GetRValue(color); green=GetGValue(color); blue=GetBValue(color);
		return; }

	//void operator=(const VString& cl){
	//	if(cl.size()!=6){ blue=0; green=0; red=0; return ; }
	//	set(cl);
	//return;}

	//void operator=(const char*line){ return set(line); return; }

	int operator==(const int color){
		if(RGB(red, green, blue)==color) return 1; return 0; }

	int operator==(const MRGB color){
		if(color.red==red && color.green==green && color.blue==blue)return 1; return 0;}

	//operator RGBQUAD*(){return (RGBQUAD*)this;}
	//operator RGBQUAD&(){return *(RGBQUAD*)&blue;}
	operator unsigned int(){ return RGB(red, green, blue); }
	//operator unsigned short(){ return sh(); }
	//unsigned short sh()const { return RGB16(*(RGBQUAD*)this); }
	//RGBQUAD shr()const { return RGB16R()this); }
	//COLORREF shc()const { return RGB16C(*(RGBQUAD*)this); }

	//MString GetString(){
	//	MString ret; ret.Reserve(6); char*r=ret;
	//	*r=red/16+( red/16 < 10 ? '0' : 55 ); *r++;
	//	*r=red%16+( red%16 < 10 ? '0' : 55 ); *r++;
	//	*r=green/16+( green/16 < 10 ? '0' : 55 ); *r++;
	//	*r=green%16+( green%16 < 10 ? '0' : 55 ); *r++;
	//	*r=blue/16+( blue/16 < 10 ? '0' : 55 ); *r++;
	//	*r=blue%16+( blue%16 < 10 ? '0' : 55 ); *r++;
	//	return ret;
	//}

};


void OpenGLDrawCircle3d(GLint x, GLint y, GLint z, GLint radius, MRGB rgb, int a=1, int lines=0){
	float angle;
	if(!lines) lines=radius+10;
	glColor3f(rgb.red/float(255), rgb.green/float(255), rgb.blue/float(255));

	glBegin(!a ? GL_LINE_LOOP : GL_POLYGON);
	for(int i = 0; i < lines; i++){
		angle = i * 2 * PI / lines;
		glVertex3f(x + (cos(angle) * radius), y + (sin(angle) * radius), z);
	}

	glEnd();
	return ;
}