#undef byte
#define byte unsigned char

enum { ZX_16, ZX_48, ZX_128, ZX_128_USR0, ZX_PLUS2, ZX_PLUS2A, ZX_PLUS3 };

extern int zx_colours[17][3];

// extern byte *zx_bordercolour, zx_bordercolours[240];

extern byte *zx_tapfile,*zx_tapfile_,*zx_tapfile_eof;
extern int   zx_pressed_play;

#define NUM_KEYB_KEYS 256

  enum SpecKeys
  {
    SPECKEY_0, SPECKEY_1, SPECKEY_2, SPECKEY_3, SPECKEY_4, SPECKEY_5,
    SPECKEY_6, SPECKEY_7, SPECKEY_8, SPECKEY_9, SPECKEY_A, SPECKEY_B,
    SPECKEY_C, SPECKEY_D, SPECKEY_E, SPECKEY_F, SPECKEY_G, SPECKEY_H,
    SPECKEY_I, SPECKEY_J, SPECKEY_K, SPECKEY_L, SPECKEY_M, SPECKEY_N,
    SPECKEY_O, SPECKEY_P, SPECKEY_Q, SPECKEY_R, SPECKEY_S, SPECKEY_T,
    SPECKEY_U, SPECKEY_V, SPECKEY_W, SPECKEY_X, SPECKEY_Y, SPECKEY_Z,
    SPECKEY_SPACE, SPECKEY_ENTER,
    SPECKEY_SHIFT, SPECKEY_SYMB, SPECKEY_CTRL
  };

  extern const unsigned char teclas_fila[NUM_KEYB_KEYS][3] ;

#define ZXKey(a) if(a>=0) fila[ teclas_fila[a][0] ][ teclas_fila[a][1] ] &= teclas_fila[a][2]
  extern  int fila[5][5];

  void ZX_Init(void);
  void ZX_Reset(int);
  void ZX_Frame(int);
  void ZX_Patch_ROM(void);
  void ZX_Unpatch_ROM(void);
  
//internal:
//  void ZX_PressPlay(void *start, void *end);
//  void ZX_StopPlay(void);
//  void ZX_SetModel(void); 
  void ZX_LoadGame(int preferred_model, unsigned long crc, int quick_if_possible);

extern byte  kempston, fuller;
extern byte  GAME[1*1024*1024]; extern long GAME_size;
extern byte *MEMr[4]; //solid block of 16*4 = 64kb for reading
extern byte *MEMw[4]; //solid block of 16*4 = 64kb for writing
extern byte  RAM_dummy[16384*1];
extern byte  RAM_pages[16384*16];
extern byte  ROM_pages[16384*4];
extern byte  model, pagination_128, pagination_plus2a, BorderColor;
//extern byte  trap_rom_loading;
extern byte tape_format;
extern void  port_0x7ffd (byte);
extern byte  cycles_delay[76000];
extern unsigned short floating_bus[76000];
extern byte  mic_on;

//#define Z80MEMWRITE(where,A) *((byte *)(MEMw[where>>14]+(where&0x3FFF)))=A
//#define Z80MEMREAD(where)    *((byte *)(MEMr[where>>14]+(where&0x3FFF)))

// nuevo
#define Z80MEMWRITE(where,A) *((byte *)(MEMw[where>>14]+(where)))=A
#define Z80MEMREAD(where)    *((byte *)(MEMr[where>>14]+(where)))

extern int ZX_LoadState(void *mem);
extern int ZX_SaveState(void *mem);
