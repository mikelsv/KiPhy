#include "ki_com.h"
#include "ki_var.h"

#define KI_PHY_POW2(v) ((v) * (v))

class KiPhyPoi{
public:
	KiVec3 pos, npos, vel;

	float &x, &y, &z;
	float &nx, &ny, &nz;
	float &vx, &vy, &vz;
	float mass, radius;
	//KiVec3 pos;
	MRGB col;

	// Init
	KiPhyPoi()
		: x(pos.x), y(pos.y), z(pos.z), nx(npos.x), ny(npos.y), nz(npos.z), vx(vel.x), vy(vel.y), vz(vel.z){		
		col.set(255, 255, 0);
	}

	void SetCol(const MRGB rgb){
		col = rgb;
	}

	void AddVec(KiVec3 v, float ms = 1){
		float m = ms / mass;
		vx += v.x * m;
		vy += v.y * m;
		vz += v.z * m;
	}

	float Length(const KiPhyPoi *poi){
		return sqrt(KI_PHY_POW2(x - poi->x) + KI_PHY_POW2(y - poi->y) + KI_PHY_POW2(z - poi->z));
	}

	KiPhyPoi& operator+=(KiVec3 v){
		x += v.x;
		y += v.y;
		z += v.z;

		return *this;
	}

	KiPhyPoi& operator-=(KiVec3 v){
		x -= v.x;
		y -= v.y;
		z -= v.z;

		return *this;
	}

	void Update(){
		radius = .01 * sqrt(mass);
	}

};

class KiPhyLink{
public:
	KiPhyPoi *p1, *p2;
	float mi, ma;
};

class KiGroup{
public:
	KiVec3 pos;
	float rotate, radius;
	int size;
	MRGB col;

	KiGroup(){
		pos = KiVec3(0, 0, 0);
		rotate = 0;
		radius = 0;
		size = 0;
		col.set(0, 0, 0);
	}
};

class KiPhy{
friend class KiYouTube1;

	// Params
	float width, height;

	// Poi
	OListSingle<KiPhyPoi> pois;
	OListSingle<KiPhyLink> links;

	// Gravity
	KiVec3 grav;

	// Mouse
	int lmouse_on, lmouse_x, lmouse_y;
	KiPhyPoi *lmouse;

public:
	// Options 
	int flag_combine;
	int flag_ki_exp_one;

public:
	void Init(float w, float h){
		width = w;
		height = h;

		lmouse_on = 0;
		flag_combine = 0;
	}

	int GetWidth(){
		return (int)width;
	}

	int GetHeight(){
		return (int)height;
	}

	void SetGravity(KiVec3 g){
		grav = g;
	}

	float Length(KiPhyPoi *p1, KiPhyPoi *p2){
		return sqrt(pow(p1->x - p2->x, 2) + pow(p1->y - p2->y, 2) + pow(p1->z - p2->z, 2));
	}

	KiVec3 Sub(KiPhyPoi *p1, KiPhyPoi *p2){
		return KiVec3(p1->x - p2->x, p1->y - p2->y, p1->z - p2->z);
	}

	KiPhyPoi* AddPoi(KiVec3 pos, float ms = 1){
		KiPhyPoi* poi = pois.New();
		poi->x = pos.x;
		poi->y = pos.y;
		poi->z = pos.z;

		//poi->vx = vel.x;
		//poi->vy = vel.y;
		//poi->vz = vel.z;

		poi->vx = 0;
		poi->vy = 0;
		poi->vz = 0;

		poi->mass = ms;
		
		poi->Update();

		//el->ela = ela;

		return poi;
	}

	KiPhyLink* AddLink(KiPhyPoi *p1, KiPhyPoi *p2, float mi, float ma){
		KiPhyLink* link = links.New();
		link->p1 = p1;
		link->p2 = p2;
		link->mi = mi;
		link->ma = ma;

		return link;
	}

	KiPhyPoi* AddBox(KiVec3 pos, KiVec2 sz, float el = 0){
		const int asz = 4;
		KiPhyPoi *poi[asz];

		poi[0] = AddPoi(pos + KiVec3(-sz.x, -sz.y, 0));
		poi[1] = AddPoi(pos + KiVec3(sz.x, -sz.y, 0));
		poi[2] = AddPoi(pos + KiVec3(sz.x, sz.y, 0));
		poi[3] = AddPoi(pos + KiVec3(-sz.x, sz.y, 0));
		//poi[4] = AddPoi(pos + KiVec3(0, 0, 0));

		for(int i = 0; i < asz; i ++){
			for(int j = i + 1; j < asz; j ++){
				//if(i != j)
				AddLink(poi[i], poi[j], 0.2 - el, 0.2 + el);
			}
		}

		return poi[0];
	}

	KiPhyPoi* AddCircle(KiVec3 pos, float radius, int sz = 0){
		KiPhyPoi *poi[256];
		if(!sz)
			sz = radius * 10;
		if(sz > 255)
			sz = 255;

		for(int i = 0; i < sz; i ++){
			float angle = i * 2 * PI / sz;
			poi[i] = AddPoi(pos + KiVec3((cos(angle) * radius), (sin(angle) * radius), 0));
		}

		poi[sz] = AddPoi(pos + KiVec3(0, 0, 0));
		sz ++;

		for(int i = 0; i < sz; i ++){
			for(int j = i + 1; j < sz; j ++){
				AddLink(poi[i], poi[j], 0.1, 0.2);
			}
		}

		return poi[0];
	}

	void AddRingGroup(KiGroup &group){
		KiPhyPoi *poi;
		for(int i = 0; i < group.size; i ++){
			float angle = i * 2 * PI / group.size + group.rotate;
			poi = AddPoi(group.pos + KiVec2((cos(angle) * group.radius), (sin(angle) * group.radius)));
			poi->AddVec(KiVec2((cos(angle) * group.radius), (sin(angle) * group.radius)));
			poi->SetCol(group.col);
		}
	}

	KiPhyPoi* NextPoi(KiPhyPoi *poi){
		return pois.Next(poi);
	}

	void Free(KiPhyPoi *poi){
		return pois.Free(poi);
	}

	void Draw(){
		glClearColor(0.00, 0.00, 0.00, 0.0);
		//glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Move elements
		KiPhyPoi *poi = 0, *poi2 = 0, *poi3;

		while(poi = pois.Next(poi)){

			//if(opt_show_vectors){
			//	glBegin(GL_LINES);
			//	 glColor3ub(180, 180, 180);
			//	 glVertex3d(el->x, el->y, el->z);
			//	 glVertex3d(el->x + el->vx * 2, el->y + el->vy * 2, el->z + el->vz * 2);
			//	 //glVertex3d(width, height, 100);
			//	 //glVertex3d(0, height, 100);
			//	 //glVertex3d(px + 50 * 1, py, pz);
			//	glEnd();
			//}

			//if(opt_show_paths){
			//	KiPhyPel *pel = pels.New();
			//	pel->x = el->x;
			//	pel->y = el->y;
			//	pel->z = el->z;
			//	pel->etime = dtime + 300;
			//}

			// Move
			float mx, my, mz;
			mx = (poi->vx - grav.x) * ki_time_delta / 1000.;
			my = (poi->vy - grav.y) * ki_time_delta / 1000.;
			mz = (poi->vz - grav.z) * ki_time_delta / 1000.;

			// Map stops
			if(poi->x + mx - poi->radius < 0 || poi->x + mx + poi->radius > 1){
				poi->vx *= -1;
				mx = 0;
			}

			if(poi->y + my - poi->radius < 0 || poi->y + my + poi->radius > 1){
				poi->vy *= -1;
				my = 0;
			}
			// End map

			poi->nx = poi->x + mx;
			poi->ny = poi->y + my;
			poi->nz = poi->z + mz;

			// Points
			poi2 = poi;

			while(poi2 = pois.Next(poi2)){
				if(/*poi != poi2 &&*/ poi->Length(poi2) <= poi->radius + poi2->radius){
					KiVec3 sub = Sub(poi, poi2), norm = sub.Normalize();
					//KiVec3 vel(poi->vx, poi->vy, poi->vz);
					float len, alen;// = vel.Length(), alen = len / norm.Length(); //(abs(norm.x) + abs(norm.y) + abs(norm.z));

					// Combine
					if(flag_combine && poi->col == poi2->col){
						poi3 = poi2;						

						float am = poi->mass + poi2->mass;
						KiVec3 p = (poi->pos * poi->mass + poi2->pos * poi2->mass) / am;
						//poi->pos += poi2->pos;
						//poi->pos /= 2.;

						len = poi->vel.Length() * poi->mass + poi2->vel.Length() * poi2->mass;
						poi->mass += poi2->mass;
						len /= poi->mass;

						norm = poi->vel + poi2->vel;
						norm.Normalize();

						alen = len / norm.Length();
						poi->vel = norm;
						poi->vel *= alen;
						//poi->vx = norm.x * alen;
						//poi->vy = norm.y * alen;
						//poi->vz = norm.z * alen;

						poi->Update();

						poi2 = pois.Prev(poi2);
						pois.Free(poi3);
					} else{
						float avel = poi->vel.Length() + poi2->vel.Length(), amass = poi->mass + poi2->mass;

						alen = avel / amass * poi2->mass /*poi->vel.Length()*/ / norm.Length();
						poi->vel = norm;
						poi->vel *= alen;

						//alen = poi2->vel.Length() / norm.Length();
						alen = avel / amass * poi->mass /*poi->vel.Length()*/ / norm.Length();
						poi2->vel = norm;
						poi2->vel *= - alen;

						//poi->vx = norm.x * alen;
						//poi->vy = norm.y * alen;
						//poi->vz = norm.z * alen;
					}
					
					//float angle = 0;
					//MglRotate(180 + angle, poi->vx, poi->vy, poi->vx, poi->vy);
				}
			}

			//if(poi->x < 0)
			//	 poi->x = 0;
			//if(poi->y < 0)
			//	 poi->y = 0;
			//if(poi->z < 0)
			//	 poi->z = 0;
		}

		// Update
		while(poi = pois.Next(poi)){
			poi->x = poi->nx;
			poi->y = poi->ny;
			poi->z = poi->nz;
		}

		// Links
		KiPhyLink *link = 0;
		while(link = links.Next(link)){
			float len = link->p1->Length(link->p2);// Length(link->p1, link->p2);
			if(len <= link->mi){
				KiVec3 sub = Sub(link->p1, link->p2), norm = sub.Normalize();
				len = len - link->mi;
				
				*link->p1 -= norm * len * .1;
				*link->p2 += norm * len * .1;
			}

			if(len >= link->ma){
				KiVec3 sub = Sub(link->p1, link->p2), norm = sub.Normalize();
				len = len - link->ma;
				
				*link->p1 -= norm * len * .1;
				*link->p2 += norm * len * .1;
			}
		}

		// Draw
		while(poi = pois.Next(poi)){
			OpenGLDrawCircle3d(poi->x * width, poi->y * height, poi->z, width * poi->radius, poi->col);

			if(flag_ki_exp_one){
				if(poi->x < 0 || poi->x > 1 || poi->y < 0 || poi->y > 1 || poi->x != poi->x || poi->y != poi->y){
					poi->pos = KiVec3(.5 + rand() * .0001, .5 + rand() * .0001, .0);
					poi->vel = KiVec3(.1, 0., 0.);
				}
 			}

		}

		while(link = links.Next(link)){
			glBegin(GL_LINES);
				glColor3ub(180, 180, 180);
				glVertex3d(link->p1->x * width, link->p1->y * height, link->p1->z);
				glVertex3d(link->p2->x * width, link->p2->y * height, link->p2->z);
			glEnd();
		}

		// Gravity
		//if(grav){
		//	while(poi = pois.Next(poi)){
		//		poi->vy += gy * ki_opt_impulse_time / 1000;
		//		//poi->y += el->vy;

		//		if(el->y >= height){
		//			el->y -= el->y - height;
		//			el->vy *= -1 * el->ela;
		//		}
		//	}
		//}

		// Move
		//if(gx || gy || gz){
		//	while(el = els.Next(el)){
		//		el->vy += gy * ki_opt_impulse_time / 1000;
		//		el->y += el->vy;

		//		if(el->y >= height){
		//			el->y -= el->y - height;
		//			el->vy *= -1 * el->ela;
		//		}
		//	}
		//}

		// Paths
		//KiPhyPel *pel = 0;
		//while(pel = pels.Next(pel)){
		//	if(pel->etime >= dtime)
		//		OpenGLDrawCircle3d(pel->x, pel->y, pel->z, 10, MRGB(155, 155, 0));
		//	else{
		//		pels.Free(pel);
		//	}
		//}

		// Mouse
		//KiMouse &mouse = KiMouse;
		if(mouse.ld && !lmouse_on){
			lmouse = 0;

			while(poi = pois.Next(poi)){
				int px = poi->x * width, py = poi->y * height, pz = 10 * poi->mass;
				if(KiInt2(px, py).Length(KiInt2(mouse.x, mouse.y)) <= pz && px > 0){
					lmouse = poi;
					break;
				}
			}

			lmouse_x = mouse.x;
			lmouse_y = mouse.y;
			lmouse_on = 1;
		} else if(mouse.ld && lmouse){
			lmouse->x = mouse.x / width;
			lmouse->y = mouse.y / height;
		} else if(!mouse.ld && lmouse_on)
			lmouse_on = 0;

	}


} ki_phy;