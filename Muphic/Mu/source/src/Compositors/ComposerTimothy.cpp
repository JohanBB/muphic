#include "Compositors/ComposerTimothy.h"

ComposerTimothy::ComposerTimothy(ComposerVoice* fm, ComposerVoice* fm2, ComposerVoice* fb, ComposerVoice* fr) :
Composer(fm, fm2, fb, fr)
{
    //ctor
}

ComposerTimothy::~ComposerTimothy()
{
    //dtor
}


string ComposerTimothy::compose()
{
	// We read the figures from the XML
	FiguresMusic* fgs = new FiguresMusic();
	fgs->cargar(pic);

	// We calculate figure visibility

	fgs->calculateVisibility();

	DURACION = fgs->getTotalVertices()*SIXTEENTHNOTE;

	// We create the pattern with whom we will compose in this composer
	//PatternGen<Figura*>* pg = new PatternGen<Figura*>();
	PriorityPattern* p = new PriorityPattern();

	// Auxiliary list to get the main figures
	std::list<FigureMusic*> padres;

	for(int i = 0; i < fgs->sizePadre(); i++)
		padres.push_back((FigureMusic*)fgs->getPadreAt(i));

	// Pattern made from the list we retrieved
	std::list<FigureMusic*> fPatronizada = p->createPatternFig(padres);

	// We calculate the duration we must assign to each figure
	std::list< std::pair<FigureMusic*, int> > aux = calcularDuracion(fPatronizada);

	// We create the scale by which we pretend to work, set by default to DOM
	MajorScale scale;
	PentatonicMajScale scalePent;
	TableScale* tbScaleBase = new TableScale(scale.getScaleSteps(), DO);
	TableScale* tbScale = new TableScale(scale.getScaleSteps(), fm->getColorSystem()->getNota(padres.front()->getRGB(), tbScaleBase));
	TableScale* tbPentScale = new TableScale(scalePent.getScaleSteps(), tbScale->getFirstNote());
	// We create the composers we will use to make the melody and rithm
	if(fm->getTypeScale() == 5)
		fm->setTableScale(tbPentScale);
	else
		fm->setTableScale(tbScale);

	if(fr->getTypeScale() == 5)
		fr->setTableScale(tbPentScale);
	else
		fr->setTableScale(tbScale);

	// We create the segments where we will put the different notes from the melody
	Segmentos* segs1 = new Segmentos();
	Segmentos* segs2 = new Segmentos();
	Segmento* seg;

	// We make the calls to the different composers with different figures sorted by vistosidad and with their timing assigned
	for(std::list< std::pair<FigureMusic*, int> >::iterator it = aux.begin(); it != aux.end(); it++)
	{
		if((*it).second > 0){
			seg = new Segmento();
			fm->compMelodyFig((*it).first, seg, (*it).second);
			segs1->pushBack(seg);

			seg = new Segmento();
			fr->compRythmFig((*it).first, seg, (*it).second);
			segs2->pushBack(seg);
		}
	}

	// We create the voices we will use in this song and we assign them the segments we created
	Voz* v1 = new Voz();
	v1->setSegmentos(segs1);
	v1->setTonalidad(DOM);
	v1->setInstrumento(fm->getInstrument());

	Voz* v2 = new Voz();
	v2->setSegmentos(segs2);
	v2->setTonalidad(DOM);
	v2->setInstrumento(DRUMS);

	Voces* vs = new Voces();

	vs->pushBack(v1);
	vs->pushBack(v2);

	// We set the parameters in music, and also we asign the voices previously created to this music
	Music* m = new Music();
	m->setComposer("Timothy1");
	m->setBaseLenght(std::make_pair(1,WHOLE));
	m->setName(getTmpMIDIPath());
	m->setVoces(vs);

	// We assign the element that will create the music from our structures
	//m->setMidizator(new MidizatorWAV());

	//m->toMidi();

	m->changeTempo(tempo);
	m->setMidizator(new MidizatorABC());
	cout << "Composition done!" << endl;
	cout << endl << "Making midi output..." << endl;
	std::string out = m->toMidi(); 

	// We make the music using the midizator previously selected
	return out;
}

string ComposerTimothy::compose(string picPath, string usrConfPath)
{
	setPic(picPath);
	setUsrConfFile(usrConfPath);

	return compose();
}

/*------Otras Funciones------*/
std::list< std::pair<FigureMusic*, int> > ComposerTimothy::calcularDuracion(std::list<FigureMusic*> f)
{
	std::list< std::pair<FigureMusic*, int> > sol;

	//Normalizamos las duraciones:

	int durBase = EIGHTHNOTE; //Duraci�n base (m�nimo indivisible)
	int dur = DURACION / durBase; // Normalizamos la duraci�n
	int durAsigned = 0; //Duraci�n que asignamos a cada figura
	int durTotal = 0;
	//std::list< std::pair<FigureMusic*, int> > sol;

	for(std::list<FigureMusic*>::iterator it = f.begin(); it != f.end(); it++)
	{
		durAsigned = floor(dur*(*it)->getVistosidad());
		sol.push_back(std::make_pair((*it),durAsigned*durBase));
		durTotal += durAsigned*durBase;
	}

	if(durTotal < DURACION)
		sol.back().second += DURACION-durTotal;

	return sol;
}

/*------Getters------*/
Conf* ComposerTimothy::getConfig()
{
	return config;
}

string ComposerTimothy::getUsrConfFile()
{
	return usrConfFile;
}

string ComposerTimothy::getPic()
{
	return pic;
}

string ComposerTimothy::getTmpMIDIPath()
{
	return tmpMIDIPath;
}

/*------Setters------*/
void ComposerTimothy::setConfig(string c)
{
	config->read(c);
}

void ComposerTimothy::setUsrConfFile(string f)
{
	usrConfFile = f;
}

void ComposerTimothy::setPic(string p)
{
	pic = p;
}

void ComposerTimothy::setTmpMIDIPath(string m)
{
	tmpMIDIPath = m.substr(0,m.find_last_of("."));
	if(tmpMIDIPath.compare("") == 0)
		tmpMIDIPath = m;
}

