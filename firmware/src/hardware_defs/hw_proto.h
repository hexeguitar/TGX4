#ifndef _HW_PROTO_H_
#define _HW_PROTO_H_

typedef enum 
{
	HW_STATE_OFF = 0,
	HW_STATE_ON,
	HW_STATE_TOGGLE
}hw_state_t;

class hw_TGX4_prototype
{
public:
	virtual bool init(void) = 0;
	virtual bool process(void) = 0;
	virtual bool dry_set(hw_state_t state) = 0;
	virtual bool dry_get(void) = 0;
	virtual bool wet_set(hw_state_t state) = 0;
	virtual bool wet_get(void) = 0;
	virtual bool state_set(hw_state_t state) = 0;
	virtual bool state_get(void) = 0;
private:

};

#endif // _HW_PROTO_H_
