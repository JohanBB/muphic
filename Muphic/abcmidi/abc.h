/* abc.h - header file shared by abc2midi, abc2abc and yaps */
/* Copyright James Allwright 2000 */
/* may be copied under the terms of the GNU public license */

typedef enum {
ABC2MIDI,
ABC2ABC,
YAPS,
ABCMATCH} programname;

/* define types of abc object */
typedef enum {
/* types of bar sign */
SINGLE_BAR,
DOUBLE_BAR,
BAR_REP,
REP_BAR,
PLAY_ON_REP,
REP1,
REP2,
/* BAR1 = SINGLE_BAR + REP1 */
/* REP_BAR2 = REP_BAR + REP2 */
BAR1,
REP_BAR2,
DOUBLE_REP,
THICK_THIN,
THIN_THICK,
/* other things */
PART,
TEMPO,
TIME,
KEY,
REST,
TUPLE,
/* CHORD replaced by CHORDON and CHORDOFF */
NOTE,
NONOTE,
OLDTIE,
TEXT,
SLUR_ON,
SLUR_OFF,
TIE,
CLOSE_TIE,
TITLE,
CHANNEL,
TRANSPOSE,
RTRANSPOSE,
GTRANSPOSE,
GRACEON,
GRACEOFF,
SETGRACE,
SETC,
SETTRIM,
GCHORD,
GCHORDON,
GCHORDOFF,
VOICE,
CHORDON,
CHORDOFF,
CHORDOFFEX,
DRUMON,
DRUMOFF,
DRONEON,
DRONEOFF,
SLUR_TIE,
TNOTE,
/* broken rhythm */
LT,
GT,
DYNAMIC,
LINENUM,
MUSICLINE,
MUSICSTOP,
WORDLINE,
WORDSTOP,
INSTRUCTION,
NOBEAM,
CHORDNOTE,
CLEF,
PRINTLINE,
NEWPAGE,
LEFT_TEXT,
CENTRE_TEXT,
VSKIP,
COPYRIGHT,
COMPOSER,
ARPEGGIO,
SPLITVOICE,
META,
PEDAL_ON,
PEDAL_OFF,
} featuretype;

/* note decorations */
#define DECSIZE 10
extern char decorations[];
#define STACCATO 0
#define TENUTO 1
#define LOUD 2
#define ROLL 3
#define FERMATA 4
#define ORNAMENT 5
#define TRILL 6
#define BOWUP 7
#define BOWDOWN 8
#define BREATH 9

/* The vstring routines provide a simple way to handle */
/* arbitrary length strings */
struct vstring {
int len;
int limit;
char* st;
};
#ifndef KANDR
/* vstring routines */
extern void initvstring(struct vstring* s);
extern void extendvstring(struct vstring* s);
extern void addch(char ch, struct vstring* s);
extern void addtext(char* text, struct vstring* s);
extern void clearvstring(struct vstring* s);
extern void freevstring(struct vstring* s);
/* error-handling routines */
extern void event_error(char *s);
extern void event_fatal_error(char *s);
extern void event_warning(char *s);
#else
/* vstring routines */
extern void initvstring();
extern void extendvstring();
extern void addch();
extern void addtext();
extern void clearvstring();
extern void freevstring();
/* error-handling routines */
extern void event_error();
extern void event_fatal_error();
extern void event_warning();
#endif

