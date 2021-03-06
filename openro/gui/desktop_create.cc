/* $Id$ */
#include "stdafx.h"

#include "desktop_create.h"

// novice
// (female)
// data\sprite\?ΰ???\??ŁE??\?ʺ???_??.act
// data\sprite\humans\body\<sex>\<novice>_<sex>
// char data[] = { 's', 'p', 'r', 'i', 't', 'e', '\\', 0xc0, 0xce, 0xb0, 0xa3, 0xc1, 0xb7, '\\', 0xb8, 0xf6, 0xc5, 0xeb, '\\', 0xbf, 0xa9, '\\', 0xc3, 0xca, 0xba, 0xb8, 0xc0, 0xda, '_', 0xbf, 0xa9, '.', 'a', 'c', 't', 0 };
// (male)
// data\sprite\?ΰ???\??ŁE??\?ʺ???_??.act
// UTF8: data\sprite\?E??E?E??\?E????\?E?\?E?보?E?_?E?.act
// char data[] = { 's', 'p', 'r', 'i', 't', 'e', '\\', 0xc0, 0xce, 0xb0, 0xa3, 0xc1, 0xb7, '\\', 0xb8, 0xf6, 0xc5, 0xeb, '\\', 0xb3, 0xb2, '\\', 0xc3, 0xca, 0xba, 0xb8, 0xc0, 0xda, '_', 0xb3, 0xb2, '.', 'a', 'c', 't', 0 };


DesktopCreate::DesktopCreate(OpenRO* ro) : RODesktop("ui\\char_create.xml", ro) {
	m_headnum = 2;
	m_palnum = 1;

	ADD_HANDLER("char_create/btnStr", evtClick, DesktopCreate::handleStr);
	ADD_HANDLER("char_create/btnAgi", evtClick, DesktopCreate::handleAgi);
	ADD_HANDLER("char_create/btnVit", evtClick, DesktopCreate::handleVit);
	ADD_HANDLER("char_create/btnInt", evtClick, DesktopCreate::handleInt);
	ADD_HANDLER("char_create/btnDex", evtClick, DesktopCreate::handleDex);
	ADD_HANDLER("char_create/btnLuk", evtClick, DesktopCreate::handleLuk);

	ADD_HANDLER("char_create/btnOk", evtClick, DesktopCreate::handleBtnOk);
	ADD_HANDLER("char_create/btnCancel", evtClick, DesktopCreate::handleBtnCancel);

	ADD_HANDLER("char_create/btnStyleDown", evtClick, DesktopCreate::handleBtnStyleDown);
	ADD_HANDLER("char_create/btnStyleUp", evtClick, DesktopCreate::handleBtnStyleUp);
	ADD_HANDLER("char_create/btnColor", evtClick, DesktopCreate::handleBtnColor);
	Str = Agi = Vit = Int = Dex = Luk = 5;

	lblStr = (GUI::Label*)getElement("char_create/str");
	lblAgi = (GUI::Label*)getElement("char_create/agi");
	lblVit = (GUI::Label*)getElement("char_create/vit");
	lblInt = (GUI::Label*)getElement("char_create/int");
	lblDex = (GUI::Label*)getElement("char_create/dex");
	lblLuk = (GUI::Label*)getElement("char_create/luk");
	window = (GUI::Window*)getElement("char_create");
	txtName = (GUI::TextInput*)getElement("char_create/name");
	updateLabels();

	readObjects();

	m_pat = 0;
	m_action = 0;
	m_dir = 0;
	elapsed = 0;
}

bool DesktopCreate::HandleKeyDown(SDL_Event *sdlEvent, int mod) {
	SDLKey key = sdlEvent->key.keysym.sym;

	if (key == SDLK_RETURN) {
		((GUI::Button*)getElement("char_create/btnOk"))->Click();
	}
	return(true);
}

bool DesktopCreate::handleBtnColor(GUI::Event&) {
	if (m_palnum == 8)
		m_palnum = 1;
	else
		m_palnum++;
	
	int accsex = m_ro->GetAccountSex();
	char fn_pal[256];
	sprintf(fn_pal, "palette\\%s\\%s%d_%s_%d.pal", ro::EUC::head2, ro::EUC::head2, m_headnum, ro::EUC::sex[accsex], m_palnum);

	FileManager& fm = m_ro->getFileManager();
	FileData data = fm.getFile(fn_pal);
	if (data.blobSize() == 0)
		return(false);

	ro::PAL* pal = new ro::PAL();
	std::stringstream ss;
	data.write(ss);
	if (!pal->readStream(ss)) {
		delete(pal);
		return(false);
	}
	char novice_head[256];
	sprintf(novice_head, "sprite\\%s\\%s\\%s\\%d_%s", ro::EUC::humans, ro::EUC::head, ro::EUC::sex[accsex], m_headnum, ro::EUC::sex[accsex]);
	CacheManager& cache = *m_ro;
	ROObjectCache& objects = cache.getROObjects();
	TextureManager& tm = cache.getTextureManager();

	std::string spr_n;
	spr_n = novice_head;
	spr_n += ".spr";
//	if (!cache.getSprGLObjects().Load(spr_n, objects, fm, pal))
	rogl::SprGL& sprgl = const_cast<rogl::SprGL&>(head.getSpr());
	const ro::SPR* spr = (ro::SPR*)objects[spr_n];
	if (!sprgl.open(spr, pal))
		return false;
	//	readObjects(pal);

	return(true);
}

bool DesktopCreate::handleBtnStyleUp(GUI::Event&) {
	m_headnum++;
	if(m_headnum >= 23)
		m_headnum = 2;

	readObjects();
	return(true);
}

bool DesktopCreate::handleBtnStyleDown(GUI::Event&) {
	m_headnum--;
	if(m_headnum < 2)
		m_headnum = 23;

	readObjects();
	return(true);
}


void DesktopCreate::readObjects(const ro::PAL* pal) {
	// body: data\sprite\<humans>\<body>\<sex>\<novice>_<sex>
	// head: data\sprite
	// 1 = archer
	// 2 = novice
	char novice_body[256];
	char novice_head[256];
	int accsex = m_ro->GetAccountSex();
	sprintf(novice_body, "sprite\\%s\\%s\\%s\\%s_%s", ro::EUC::humans, ro::EUC::body, ro::EUC::sex[accsex], ro::EUC::novice, ro::EUC::sex[accsex]);
	sprintf(novice_head, "sprite\\%s\\%s\\%s\\%d_%s", ro::EUC::humans, ro::EUC::head, ro::EUC::sex[accsex], m_headnum, ro::EUC::sex[accsex]);
	printf("Novice body: %s\n", novice_body);
	printf("Novice head: %s\n", novice_head);

	obj.Load(novice_body, *m_ro);
	obj.setPos(94, 180, 0);

	if (!head.Load(novice_head, *m_ro, pal)) {
	//	m_headnum = 2;
		sprintf(novice_head, "sprite\\%s\\%s\\%s\\%d_%s", ro::EUC::humans, ro::EUC::head, ro::EUC::sex[accsex], m_headnum,ro::EUC::sex[accsex]);
		head.Load(novice_head, *m_ro, pal);
	}
}


void DesktopCreate::afterDraw(unsigned int delay) {
/*	static float str_coords[][2] = {
		{ 288.0f, 166.0f },
		{ 288.0f, 150.0f },
		{ 288.0f, 142.0f },
		{ 288.0f, 134.0f },
		{ 288.0f, 126.0f },
		{ 288.0f, 118.0f },
		{ 288.0f, 110.0f },
		{ 288.0f, 102.0f },
		{ 288.0f,  94.0f },
		{ 288.0f,  86.0f }
	};

	static float int_coords[][2] = {
		{ 288.0f, 166.0f },
		{ 288.0f, 174.0f },
		{ 288.0f, 182.0f },
		{ 288.0f, 190.0f },
		{ 288.0f, 198.0f },
		{ 288.0f, 206.0f },
		{ 288.0f, 214.0f },
		{ 288.0f, 222.0f },
		{ 288.0f, 230.0f },
		{ 288.0f, 246.0f }
	};

	static float vit_coords[][2] = {
		{ 288.0f, 166.0f },
		{ 294.8f, 161.9f },
		{ 301.6f, 157.8f },
		{ 308.4f, 153.7f },
		{ 315.2f, 149.6f },
		{ 322.0f, 145.5f },
		{ 328.8f, 141.4f },
		{ 335.6f, 137.3f },
		{ 342.4f, 133.2f },
		{ 356.0f, 125.0f }
	};

	static float dex_coords[][2] = {
		{ 288.0f, 166.0f },
		{ 281.2f, 170.0f },
		{ 274.4f, 174.0f },
		{ 267.6f, 178.0f },
		{ 260.8f, 182.0f },
		{ 254.0f, 186.0f },
		{ 247.2f, 190.0f },
		{ 240.4f, 194.0f },
		{ 233.6f, 198.0f },
		{ 220.0f, 206.0f }
	};

	static float agi_coords[][2] = {
		{ 288.0f, 166.0f },
		{ 281.2f, 162.0f },
		{ 274.4f, 158.0f },
		{ 267.6f, 154.0f },
		{ 260.8f, 150.0f },
		{ 254.0f, 146.0f },
		{ 247.2f, 142.0f },
		{ 240.4f, 138.0f },
		{ 233.6f, 134.0f },
		{ 220.0f, 126.0f }
	};

	static float luk_coords[][2] = {
		{ 288.0f, 166.0f },
		{ 294.6f, 170.0f },
		{ 301.6f, 174.0f },
		{ 308.4f, 178.0f },
		{ 315.2f, 182.0f },
		{ 322.0f, 186.0f },
		{ 328.8f, 190.0f },
		{ 335.6f, 194.0f },
		{ 342.4f, 198.0f },
		{ 356.0f, 206.0f }
	};*/
	static float str_coords[10][2];
	static float int_coords[10][2];
	static float vit_coords[10][2];
	static float dex_coords[10][2];
	static float agi_coords[10][2];
	static float luk_coords[10][2];

	for (int i = 0; i < 10; i++)
	{
		str_coords[i][0] = 288.0f;
		int_coords[i][0] = 288.0f;
		vit_coords[i][0] = 288.0f + (i + 1) * 6.8f;
		dex_coords[i][0] = 288.0f - (i + 1) * 6.8f;
		agi_coords[i][0] = 288.0f - (i + 1) * 6.8f;
		luk_coords[i][0] = 288.0f + (i + 1) * 6.8f;

		str_coords[i][1] = 166.0f - (i + 1) * 8.f;
		int_coords[i][1] = 166.0f + (i + 1) * 8.f;
		vit_coords[i][1] = 166.0f - (i + 1) * 4.f;
		dex_coords[i][1] = 166.0f + (i + 1) * 4.f;
		agi_coords[i][1] = 166.0f - (i + 1) * 4.f;
		luk_coords[i][1] = 166.0f + (i + 1) * 4.f;
	/*	str_coords[i][0] = 288.0ff;
		int_coords[i][0] = 288.0ff;
		vit_coords[i][0] = 288.0f + 3.2 + (i + 1) * 6.48f;
		dex_coords[i][0] = 288.0f - 3.2 - (i + 1) * 6.48f;
		agi_coords[i][0] = 288.0f - 3.2 - (i + 1) * 6.48f;
		luk_coords[i][0] = 288.0f + 3.2 + (i + 1) * 6.48f;

		str_coords[i][1] = 166.0f - 4 - (i + 1) * 7.6f;
		int_coords[i][1] = 166.0f + 4 + (i + 1) * 7.6f;
		vit_coords[i][1] = 166.0f - 2 - (i + 1) * 3.8f;
		dex_coords[i][1] = 166.0f + 2 + (i + 1) * 3.8f;
		agi_coords[i][1] = 166.0f - 2 - (i + 1) * 3.8f;
		luk_coords[i][1] = 166.0f + 2 + (i + 1) * 3.8f;*/
	}

	glPushMatrix();
	glTranslatef((float)window->getX(), (float)window->getY(), 0);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glColor4d(0.482352941, 0.580392157, 0.803921569, window->m_opacity); // soft blue from the original client
	glBegin(GL_TRIANGLE_FAN);

	//		START			MID			END
	// STR	(288, 88) => (288, 125) => (288, 166)
	// VIT				 (323, 145)
	// LUK				 (321, 186)
	// INT				 (288, 208)
	// DEX				 (249, 190)
	// AGI				 (250, 145)
	glVertex3f(288.0f, 166.0f, 0.0f); // Center
	glVertex3f(str_coords[Str][0], str_coords[Str][1], 0);	// STR
	glVertex3f(vit_coords[Vit][0], vit_coords[Vit][1], 0);	// VIT
	glVertex3f(luk_coords[Luk][0], luk_coords[Luk][1], 0);	// LUK
	glVertex3f(int_coords[Int][0], int_coords[Int][1], 0);	// INT
	glVertex3f(dex_coords[Dex][0], dex_coords[Dex][1], 0);	// DEX
	glVertex3f(agi_coords[Agi][0], agi_coords[Agi][1], 0);	// AGI
	glVertex3f(str_coords[Str][0], str_coords[Str][1], 0);	// STR -- round trip

	glEnd();
	glColor4f(1,1,1,1);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	elapsed += delay;
	ptick += delay;

	drawChar();

	glPopMatrix();

	//Keep Alive packet to CharServer
	if(ptick >= 12000){
		m_ro->KeepAliveChar();
		ptick = 0;
	}

	//m_ro->ProcessMouse();
}

void cross(float x, float y, float size) {
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glVertex3f(x - size, y,0);
	glVertex3f(x + size, y,0);
	glVertex3f(x, y - size,0);
	glVertex3f(x, y + size,0);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

void DesktopCreate::drawChar() {
	float x, y;

	x = 94;
	y = 217;
	float dif = 50;

	glColor3f(1, 0, 0);

	glBegin(GL_LINES);
	glVertex3f(x-dif, y,0);
	glVertex3f(x+dif, y,0);
	glVertex3f(x, y-dif,0);
	glVertex3f(x, y+dif,0);
	glEnd();

	glColor3f(1, 1, 1);


	while (elapsed > 600) {
		m_act = m_action * 8 + m_dir;
		const ro::ACT::Action& cact = obj->getAction(m_act);
		elapsed -= 600;
		m_dir++;
		if (m_dir >= 8)
			m_dir = 0;
	/*	m_pat++;
		if (m_pat >= cact.getMotionCount()) {
			m_pat = 0;
			m_dir++;
			if (m_dir >= 8)
				m_dir = 0;
		}*/
	}

	m_act = m_action * 8 + m_dir;
	const ro::ACT::Motion& cmot = obj->getMotion(m_act, m_pat);
	const ro::ACT::Motion& cmot2 = head->getMotion(m_act, m_pat);

//	drawAct(x, y, obj, m_pat);
//	drawAct(x, y, obj, m_pat, true);

	// EXT position
	float cx = x;
	float cy = y;
	drawAct(cx, cy, obj, m_pat);

	//cx += cpat[0].w / 2;

#if 0
	glColor3f(0, 1, 0);
	cross(cx, cy, dif);
	glColor3f(1, 1, 1);
#endif

	if (cmot.attachPoints.size() > 0) {
		cx += cmot.attachPoints[0].x;
		cy += cmot.attachPoints[0].y;
	}
	if (cmot2.attachPoints.size() > 0) {
		cx -= cmot2.attachPoints[0].x;
		cy -= cmot2.attachPoints[0].y;
	}

	drawAct(cx, cy, head, m_pat, false); // << Work most of the times
//	drawAct(cx, cy, head, m_pat, true); // << Work most of the times

#if 0
	glColor3f(0, 0, 1);
	cross(cx, cy, dif);
	glColor3f(1, 1, 1);
#endif
}

void DesktopCreate::drawAct(float x, float y, GuiAct& o, int pat, bool ext) {
	const ro::ACT::Motion& cmot = o->getMotion(m_act, pat);
	
	unsigned int i;

	for (i = 0; i < cmot.getClipCount(); i++)
		drawSpr(x, y, o, pat, i, ext);
}

void DesktopCreate::drawSpr(float x, float y, GuiAct& o, int pat, int spr, bool ext) {
	//float w, h;
	//float u[2], v[2];
	const ro::ACT::Motion& cmot = o->getMotion(m_act, pat);
	//sdle::Texture tp;

//	o.getSpr().Draw(cmot, spr, x, y, false, ext);
	o.getSpr().Draw(cmot, spr, x, y, true, ext, false, window->m_opacity);

	/*
	if (cpat[spr].sprNo < 0)
		return;
	const ro::ACT::Spr& cspr = cpat[cpat[spr].sprNo];
	tp = o.getTextures()[cpat[spr].sprNo];
	if (!tp.Valid())
		return;
	if (cspr.w > 0) {
		w = (float)cspr.w;
		h = (float)cspr.h;
	}
	else {
		w = (float)tp.getWidth();
		h = (float)tp.getHeight();
	}

	w *= cpat[spr].xMag;
	h *= cpat[spr].yMag;

	bool mirror = (cspr.mirrorOn != 0);

	u[0] = 0.0f;
	u[1] = tp->getMaxU();

	v[0] = 0.0f;
	v[1] = tp->getMaxV();

	if (mirror) {
		w = -w;
	}

	x += cpat[0].x;
	y += cpat[0].y;

	if (ext) {
		x -= cpat.ext_x;
		y -= cpat.ext_y;
	}

	tp.Activate();

	glBegin(GL_QUADS);
	glTexCoord2f(u[0], v[0]);
	glVertex3f(x - w/2, y - h/2, 0);
	glTexCoord2f(u[0], v[1]);
	glVertex3f(x - w/2, y + h/2, 0);
	glTexCoord2f(u[1], v[1]);
	glVertex3f(x + w/2, y + h/2, 0);
	glTexCoord2f(u[1], v[0]);
	glVertex3f(x + w/2, y - h/2, 0);
	glEnd();

#if 0
	glDisable(GL_TEXTURE_2D);
	glColor3f(0,1,0);
	glBegin(GL_LINES);
	glVertex3f(x - w/2, y - h/2, 0);
	glVertex3f(x - w/2, y + h/2, 0);

	glVertex3f(x - w/2, y + h/2, 0);
	glVertex3f(x + w/2, y + h/2, 0);

	glVertex3f(x + w/2, y + h/2, 0);
	glVertex3f(x + w/2, y - h/2, 0);

	glVertex3f(x + w/2, y - h/2, 0);
	glVertex3f(x - w/2, y - h/2, 0);
	glEnd();
	glColor3f(1,1,1);
	glEnable(GL_TEXTURE_2D);
#endif
	*/
}



void DesktopCreate::updateLabels() {
	char buf[16];
	
	sprintf(buf, "%d", Str);
	lblStr->setText(buf);

	sprintf(buf, "%d", Agi);
	lblAgi->setText(buf);

	sprintf(buf, "%d", Vit);
	lblVit->setText(buf);

	sprintf(buf, "%d", Int);
	lblInt->setText(buf);

	sprintf(buf, "%d", Dex);
	lblDex->setText(buf);

	sprintf(buf, "%d", Luk);
	lblLuk->setText(buf);
}

bool DesktopCreate::handleStr(GUI::Event&) {
	if (Int == 1)
		return(true);
	Str++;
	Int--;
	updateLabels();
	return(true);
}

bool DesktopCreate::handleInt(GUI::Event&) {
	if (Str == 1)
		return(true);
	Str--;
	Int++;
	updateLabels();
	return(true);
}

bool DesktopCreate::handleVit(GUI::Event&) {
	if (Dex == 1)
		return(true);
	Vit++;
	Dex--;
	updateLabels();
	return(true);
}

bool DesktopCreate::handleDex(GUI::Event&) {
	if (Vit == 1)
		return(true);
	Dex++;
	Vit--;
	updateLabels();
	return(true);
}

bool DesktopCreate::handleAgi(GUI::Event&) {
	if (Luk == 1)
		return(true);
	Agi++;
	Luk--;
	updateLabels();
	return(true);
}

bool DesktopCreate::handleLuk(GUI::Event&) {
	if (Agi == 1)
		return(true);
	Luk++;
	Agi--;
	updateLabels();
	return(true);
}

bool DesktopCreate::handleBtnOk(GUI::Event&) {
	CharAttributes attr;

	attr.Str = Str;
	attr.Agi = Agi;
	attr.Vit = Vit;
	attr.Int = Int;
	attr.Dex = Dex;
	attr.Luk = Luk;

	m_ro->CreateChar(txtName->getText(),attr,slot,0,m_headnum);
	clear();

	return(true);
}

void DesktopCreate::clear() {
	m_headnum = 2;
	txtName->setText("");
	Str = Agi = Vit = Int = Dex = Luk = 5;
	updateLabels();
}

bool DesktopCreate::handleBtnCancel(GUI::Event&) {
//	clear();
	m_ro->CharSelectScreen();
	return(true);
}

int DesktopCreate::getStr() const { return(Str); }
int DesktopCreate::getAgi() const { return(Agi); }
int DesktopCreate::getVit() const { return(Vit); }
int DesktopCreate::getInt() const { return(Int); }
int DesktopCreate::getDex() const { return(Dex); }
int DesktopCreate::getLuk() const { return(Luk); }

const std::string& DesktopCreate::getName() const {
	return(txtName->getText());
}
