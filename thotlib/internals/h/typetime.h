typedef struct _timeRec
{
	int             tmsec;	/* secondes */
	int             tmmin;	/* minutes */
	int             tmhour;	/* heures (0-23) */
	int             tmmday;	/* jour du mois (1-31) */
	int             tmmon;	/* mois de l'annee (0-11) */
	int             tmyear;	/* annee - 1900 */
	int             tmwday;	/* jour de la semaine (Dimanche=0) */
	int             tmyday;	/* jour de l'annee (0-365) */
	int             tmisdst;
}               timeRec;

typedef struct _timeRec *PtrTime;
typedef int    *wpt;
