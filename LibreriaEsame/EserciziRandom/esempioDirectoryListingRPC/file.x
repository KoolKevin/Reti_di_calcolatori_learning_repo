const MAXNAMELEN = 255;

typedef string nametype <MAXNAMELEN>;

typedef struct namenode *namelist;
struct namenode {nametype name; namelist next;};

/* risultato del servizio RLS */
union readdir_res switch (int errno){
	case 0: namelist list;
	default: void;
};

program RLSPROG {
	version RLSVERS {
		readdir_res READDIR (nametype)=1;
	} = 1;
} = 0x20000013;
