/*
 * This file is part of the UCB release of Plan 9. It is subject to the license
 * terms in the LICENSE file found in the top-level directory of this
 * distribution and at http://akaros.cs.berkeley.edu/files/Plan9License. No
 * part of the UCB release of Plan 9, including this file, may be copied,
 * modified, propagated, or distributed except according to the terms contained
 * in the LICENSE file.
 */

/*
 * sha1sum - compute SHA1 or SHA2 digest
 */
#include <u.h>
#include <libc.h>
#include <bio.h>
#include <libsec.h>


typedef struct Sha2 Sha2;
struct Sha2 {
	int	bits;
	int	dlen;
	DigestState* (*func)(uint8_t *, uint32_t, uint8_t *, DigestState *);
};

static Sha2 sha2s[] = {
	224,	SHA2_224dlen,	sha2_224,
	256,	SHA2_256dlen,	sha2_256,
	384,	SHA2_384dlen,	sha2_384,
	512,	SHA2_512dlen,	sha2_512,
};

static DigestState* (*shafunc)(uint8_t *, uint32_t, uint8_t *,
			       DigestState *);
static int shadlen;

static int
digestfmt(Fmt *fmt)
{
	char buf[SHA2_512dlen*2 + 1];
	uint8_t *p;
	int i;

	p = va_arg(fmt->args, uint8_t*);
	for(i = 0; i < shadlen; i++)
		sprint(buf + 2*i, "%.2x", p[i]);
	return fmtstrcpy(fmt, buf);
}

static void
sum(int fd, char *name)
{
	int n;
	uint8_t buf[8192], digest[SHA2_512dlen];
	DigestState *s;

	s = (*shafunc)(nil, 0, nil, nil);
	while((n = read(fd, buf, sizeof buf)) > 0)
		(*shafunc)(buf, n, nil, s);
	if(n < 0){
		fprint(2, "reading %s: %r\n", name? name: "stdin");
		return;
	}
	(*shafunc)(nil, 0, digest, s);
	if(name == nil)
		print("%M\n", digest);
	else
		print("%M\t%s\n", digest, name);
}

static void
usage(void)
{
	fprint(2, "usage: %s [-2 bits] [file...]\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	int i, fd, bits;
	Sha2 *sha;

	shafunc = sha1;
	shadlen = SHA1dlen;
	ARGBEGIN{
	case '2':
		bits = atoi(EARGF(usage()));
		for (sha = sha2s; sha < sha2s + nelem(sha2s); sha++)
			if (sha->bits == bits)
				break;
		if (sha >= sha2s + nelem(sha2s))
			sysfatal("unknown number of sha2 bits: %d", bits);
		shafunc = sha->func;
		shadlen = sha->dlen;
		break;
	default:
		usage();
	}ARGEND

	fmtinstall('M', digestfmt);

	if(argc == 0)
		sum(0, nil);
	else
		for(i = 0; i < argc; i++){
			fd = open(argv[i], OREAD);
			if(fd < 0){
				fprint(2, "%s: can't open %s: %r\n", argv0, argv[i]);
				continue;
			}
			sum(fd, argv[i]);
			close(fd);
		}
	exits(nil);
}
