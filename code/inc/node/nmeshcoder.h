#ifndef N_MESH_CODER_H
#define N_MESH_CODER_H

#include "mathlib/vector.h"

#define  VMAX_X  2047.0f
#define  VMAX_Y  1023.0f
#define  VMAX_Z  2047.0f

#define  X_SHIFT 21
#define  Y_SHIFT 11
#define  Z_SHIFT 0

#define  X_MASK  0xFFE00000
#define  Y_MASK  0x001FF800
#define  Z_MASK  0x000007FF

#define  GET_X(u)  (((u) & X_MASK) >> X_SHIFT)
#define  GET_Y(u)  (((u) & Y_MASK) >> Y_SHIFT)
#define  GET_Z(u)  ((u) & Z_MASK)

#define  PUT_X(u, x)  u |= (((unsigned long)((x) + 0.5f)) << X_SHIFT)
#define  PUT_Y(u, y)  u |= (((unsigned long)((y) + 0.5f)) << Y_SHIFT)
#define  PUT_Z(u, z)  u |= ((unsigned long)((z) + 0.5f))

#define  TMAX_U  255.0f
#define  TMAX_V  255.0f

#define  U_SHIFT 8
#define  V_SHIFT 0
#define  U_MASK  0xFF00
#define  V_MASK  0x00FF

#define  GET_U(u)  (((u) & U_MASK) >> U_SHIFT)
#define  GET_V(u)  ((u) & V_MASK)

#define  PUT_U(t, u)  t |= (((unsigned short)((u) + 0.5f)) << U_SHIFT)
#define  PUT_V(t, v)  t |= ((unsigned short)((v) + 0.5f))

#define  WMAX  255.0f

struct nMeshCoder {

    unsigned long encode_v(vector3& v) {
        unsigned long u = 0;
        PUT_X(u, (v.x + vt.x) * vs.x);
        PUT_Y(u, (v.y + vt.y) * vs.y);
        PUT_Z(u, (v.z + vt.z) * vs.z);
        return u;
    }

    void decode_v(unsigned long u, vector3& v) {
        
        v.x = ((float)GET_X(u)) / vs.x - vt.x;
        v.y = ((float)GET_Y(u)) / vs.y - vt.y;
        v.z = ((float)GET_Z(u)) / vs.z - vt.z;
    }        

    unsigned short encode_t(vector2& t) {
        unsigned short r = 0;
        PUT_U(r, (t.x + tt.x) * ts.x);
        PUT_V(r, (t.y + tt.y) * ts.y);        
        return r;
    }

    void decode_t(unsigned short r, vector2& t) {        
        t.x = ((float)GET_U(r)) / ts.x - tt.x;
        t.y = ((float)GET_V(r)) / ts.y - tt.y;        
    }    

    unsigned char encode_w(float w) {        
        return (unsigned char)((w + wt) * ws);        
    }

    float decode_w(unsigned char w) {        
        return ((float)w) / ws - wt;        
    }    

    void eval_v(vector3 vmin, vector3 vmax) {
        this->vt = -vmin;
        vmax -= vmin;
        this->vs.x = VMAX_X/vmax.x;
        this->vs.y = VMAX_Y/vmax.y;
        this->vs.z = VMAX_Z/vmax.z;
    }

    void eval_t(vector2 tmin, vector2 tmax) {
        this->tt = -tmin;
        tmax -= tmin;
        this->ts.x = TMAX_U/tmax.x;
        this->ts.y = TMAX_V/tmax.y;        
    }

    void eval_w(float wmin, float wmax) {
        this->wt = -wmin;
        wmax -= wmin;

        this->ws = WMAX/wmax;        
    }
    
    char encode_n(float c) {
        return (char)((c + 0.5f) * 127.0f);
    }

    float decode_n(char c) {
        return ((float)c)/127.0f;                
    }
    
// compression data
    vector3     vs;
    vector3     vt;

    vector2     ts;
    vector2     tt;

    float       wt;
    float       ws;
};

#endif