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
	MRGB(const RGBQUAD cl){ red=cl.rgbRed; green=cl.rgbGreen; blue=cl.rgbBlue; }

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

	operator RGBQUAD*(){return (RGBQUAD*)this;}
	operator RGBQUAD&(){return *(RGBQUAD*)&blue;}
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