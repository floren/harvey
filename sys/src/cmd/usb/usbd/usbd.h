typedef struct Hub Hub;
typedef struct Port Port;
typedef struct DHub DHub;
typedef struct Devtab Devtab;
typedef struct Usbfs Usbfs;

enum
{
	Stack	= 32*1024,

	Dhub	= 0x29,		/* hub descriptor type */
	Dhublen = 9,		/* hub descriptor length */

	/* hub class feature selectors */
	Fhublocalpower	= 0,
	Fhubovercurrent	= 1,

	Fportconnection	= 0,
	Fportenable	= 1,
	Fportsuspend	= 2,
	Fportovercurrent = 3,
	Fportreset	= 4,
	Fportpower	= 8,
	Fportlowspeed	= 9,
	Fcportconnection	= 16,
	Fcportenable	= 17,
	Fcportsuspend	= 18,
	Fcportovercurrent= 19,
	Fcportreset	= 20,
	Fportindicator	= 22,

	/* Port status and status change bits
	 * Constants at /sys/src/9/pc/usb.h starting with HP-
	 * must have the same values or root hubs won't work.
	 */
	PSpresent	= 0x0001,
	PSenable	= 0x0002,
	PSsuspend	= 0x0004,
	PSovercurrent	= 0x0008,
	PSreset		= 0x0010,
	PSpower		= 0x0100,
	PSslow		= 0x0200,
	PShigh		= 0x0400,

	PSstatuschg	= 0x10000,	/* PSpresent changed */
	PSchange	= 0x20000,	/* PSenable changed */


	/* port/device state */
	Pdisabled = 0,		/* must be 0 */
	Pattached,
	Pconfiged,

	/* Delays, timeouts (ms) */
	Spawndelay	= 100,		/* how often may we re-spawn a driver */
	Connectdelay	= 500,		/* how much to wait after a connect */
	Resetdelay	= 20,		/* how much to wait after a reset */
	Enabledelay	= 20,		/* how much to wait after an enable */
	Powerdelay	= 100,		/* after powering up ports */
	Pollms		= 250, 		/* port poll interval */
	Chgdelay	= 100,		/* waiting for port become stable */
	Chgtmout	= 1000,		/* ...but at most this much */

	/*
	 * device tab for embedded usb drivers.
	 */
	DCL = 0x01000000,		/* csp identifies just class */
	DSC = 0x02000000,		/* csp identifies just subclass */
	DPT = 0x04000000,		/* csp identifies just proto */

};

struct Hub
{
	u8	pwrmode;
	u8	compound;
	u8	pwrms;		/* time to wait in ms */
	u8	maxcurrent;	/*    after powering port*/
	int	leds;		/* has port indicators? */
	int	maxpkt;
	u8	nport;
	Port	*port;
	int	failed;		/* I/O error while enumerating */
	int	isroot;		/* set if root hub */
	Dev	*dev;		/* for this hub */
	Hub	*next;		/* in list of hubs */
};

struct Port
{
	int	state;		/* state of the device */
	int	sts;		/* old port status */
	u8	removable;
	u8	pwrctl;
	Dev	*dev;		/* attached device (if non-nil) */
	Hub	*hub;		/* non-nil if hub attached */
	int	devnb;		/* device number */
	u64	*devmaskp;	/* ptr to dev mask */
};


/* USB HUB descriptor */
struct DHub
{
	u8	bLength;
	u8	bDescriptorType;
	u8	bNbrPorts;
	u8	wHubCharacteristics[2];
	u8	bPwrOn2PwrGood;
	u8	bHubContrCurrent;
	u8	DeviceRemovable[1];	/* variable length */
};

struct Devtab
{
	char	*name;
	int	(*init)(Dev*, int, char**);	/* nil if external */
	int	csps[4];
	int	vid;
	int	did;
	char	*args;
	u64	devmask;
	int	noauto;
};


Hub*	newhub(char *fn, Dev *d);
int	startdev(Port *pp);
int	getdevnb(u64 *maskp);
void	putdevnb(u64 *maskp, int nb);
void	threadmain(int argc, char **argv);

extern Usbfs usbdfsops;
