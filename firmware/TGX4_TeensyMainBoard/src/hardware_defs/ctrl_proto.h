#ifndef _CTRL_PROTO_H_
#define _CTRL_PROTO_H_

class ctrl_TGX4_prototype
{
public:
	virtual void init(void) = 0;
	virtual void process(void) = 0;
	virtual void signalChange(void) = 0;
private:
	
};

class ctrl_TGX4_dummy : public ctrl_TGX4_prototype
{
public:
	ctrl_TGX4_dummy(){};
	void init(void){};
	void process(void){};
	void signalChange(void){};
private:
};

#if defined (USE_CTRL_DUMMY)
	extern ctrl_TGX4_dummy gui;
#endif

#endif // _CTRL_PROTO_H_
