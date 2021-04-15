typedef struct Usbfs Usbfs;
typedef struct Fid Fid;

enum
{
	Hdrsize	= 128,		/* plenty of room for headers */
	Msgsize	= 8216,		/* our preferred iounit (also devmnt's) */
	Bufsize	= Hdrsize + Msgsize,
	Namesz = 40,
	Errmax = 128,
	ONONE = ~0,		/* omode in Fid when not open */
};

struct Fid
{
	int	fid;
	Qid	qid;
	int	omode;
	Fid*	next;
	void*	aux;
};

struct Usbfs
{
	char	name[Namesz];
	u64	qid;
	Dev*	dev;
	void*	aux;

	int	(*walk)(Usbfs *fs, Fid *f, char *name);
	void	(*clone)(Usbfs *fs, Fid *of, Fid *nf);
	void	(*clunk)(Usbfs *fs, Fid *f);
	int	(*open)(Usbfs *fs, Fid *f, int mode);
	i32	(*read)(Usbfs *fs, Fid *f, void *data, i32 count,
			    i64 offset);
	i32	(*write)(Usbfs *fs, Fid*f, void *data, i32 count,
			     i64 offset);
	int	(*stat)(Usbfs *fs, Qid q, Dir *d);
	void	(*end)(Usbfs *fs);
};

typedef int (*Dirgen)(Usbfs*, Qid, int, Dir*, void*);

i32	usbreadbuf(void *data, i32 count, i64 offset, void *buf, i32 n);
void	usbfsadd(Usbfs *dfs);
void	usbfsdel(Usbfs *dfs);
int	usbdirread(Usbfs*f, Qid q, char *data, i32 cnt, i64 off,
		      Dirgen gen, void *arg);
void	usbfsinit(char* srv, char *mnt, Usbfs *f, int flag);

void	usbfsdirdump(void);

extern char Enotfound[];
extern char Etoosmall[];
extern char Eio[];
extern char Eperm[];
extern char Ebadcall[];
extern char Ebadfid[];
extern char Einuse[];
extern char Eisopen[];
extern char Ebadctl[];

extern Usbfs usbdirfs;
extern int usbfsdebug;
