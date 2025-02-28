#include "Compositors/ComposerFigMelody.h"

/*------Constructoras------*/
ComposerFigMelody::ComposerFigMelody(ColorSystem* cs) : ComposerVoice(cs)
{
	// ctor
	typeScale = 5;
}

ComposerFigMelody::ComposerFigMelody(ColorSystem* cs, TableScale* tbScale) : ComposerVoice(cs, tbScale)
{
	// ctor
	typeScale = 5;
}

/*------Destructora------*/
ComposerFigMelody::~ComposerFigMelody()
{
	//// dtor
	//delete tb;
	//tb = NULL;
}

//Hacemos una melodia de duracion limitada y que sea a partir de la figura dada.
bool ComposerFigMelody::compMelodyFig(FigureMusic* f, Segmento* seg, int dur, int maxDur, int minDur)
{
	//Vemos el color y en que nos movemos.
	Color color = f->getRGB();

	int nota = cs->getNota(color);

	//Ordenamos los vertices de la figura teniendo en cuenta el focus... (nada por ahora) CAMBIAR!
	vector< Vertice* > vertices;
	int numVertices = f->getNumVertices();
	for(int i = 0; i < numVertices; i++)
		vertices.push_back(f->getVerticeAt(i));

	//Componemos:

		// Duracion ***************

	//Tratamos primero el problema de la duraci�n: Vamos a asociar longitudes entre vertices con la duraci�n total que disponemos
	int durActual = 0; //Duraci�n que llevamos usado
	int durBase = 1; //Duraci�n base que vamos a usar
	bool canSubdivide = true;
	while(canSubdivide && durBase < SIXTEENTHNOTE)
	{
		canSubdivide = mod(dur,durBase*2) == 0;
		if(canSubdivide)
			durBase = durBase*2;
	}

	//Calculamos la longitud total de la figura
	double distTotal = 0;
	vector< double > distWithNext;  //Distancias con el siguiente vertice.
	for(int i = 0; i < numVertices; i++)
	{
		distWithNext.push_back( dist2DPoints(vertices.at(i)->getPair(), vertices.at((i+1)%numVertices)->getPair()) );
		distTotal += distWithNext.back();
	}
	//Ahora vemos la proporci�n duraci�n-Distancia 1 DurBase = X Dist
	double proporDurDist = distTotal / dur;
	proporDurDist = proporDurDist * durBase;

	vector< int > durVertice; //Las duraciones que dispone cada v�rtice
	//Primero asignaci�n de duraciones iniciales, luego lo recalibramos para usar toda la duraci�n disponible
	double fractPart;
	double intPart;
	for(int i = 0; i < numVertices; i++)
	{
		fractPart = modf(distWithNext.at(i) / proporDurDist, &intPart);
		durVertice.push_back((int)intPart*durBase);
		durActual += (int)intPart*durBase;
		distWithNext.at(i) = fractPart; //Dejamos la fraccion que nos ha quedado para los reajustes
	}

	//Reajustes de la duracion. Simplemente se reasigna toda la duraci�n que falta. Se puede mejorar con un filtro de poner cosas inteligentes (no negra+semifusa)
	int candidato = 0;
	double propor = 0.0;
	while(durActual < dur)
	{
		candidato = 0;
		propor = distWithNext.at(candidato);
		for(int i = 0; i < numVertices; i++){
			if(propor < distWithNext.at(i))		//Cogemos al que mayor fracci�n de decimales le sali�
			{
				candidato = i;
				propor = distWithNext.at(i);
			}
		}
		distWithNext.at(candidato) -= proporDurDist;
		durVertice.at(candidato) += durBase;
		durActual += durBase;
	}

		//Tono **************

	//Ahora segun la duracion que nos han dado y los angulos que se forman con las aristas de la figura a�adimos notas
	double angle;
	int step;	// El cambio de tono que vamos a hacer.
	Nota* lastNote,* note;
	//La primera nota es el color de la figura (no disponemos de m�s info)
	lastNote = new Nota(durVertice.at(0), cs->getNota(color));
	if(lastNote->getDuracion() > 0)
		seg->pushBack(lastNote);
	//Ahora el resto de v�rtices desde 1 a numVertices
	for(int i = 1; i < numVertices; i++)
	{
		angle = angleOf2Lines2(vertices.at((i+1)%numVertices)->getPair(), vertices.at(i)->getPair(), vertices.at(mod((i-1),numVertices))->getPair(), vertices.at(i)->getPair());
		step = (int) floor(sin(angle*(PI/180))*2.5); //Como mucho dejamos que de un salto de 2 tonos-Escala (que no semitonos)

		if((step == 3 || step == -3) && durVertice.at(i) > QUARTERNOTE+EIGHTHNOTE-1) //Usamos una nota intermedia (lo pide a gritos XD)
		{
			if(durVertice.at(i) > HALFNOTE)
			{
				if(step > 0)
					note = new Nota(QUARTERNOTE, tb->nextTone(lastNote->getTono()));
				else
					note = new Nota(QUARTERNOTE, tb->prevTone(lastNote->getTono()));
				seg->getSimbolos()->pushBack(note); //a�adimos la nota intermedia

				if(step > 0)
					note = new Nota(durVertice.at(i)-QUARTERNOTE, tb->nextNTone(lastNote->getTono(),3));
				else
					note = new Nota(durVertice.at(i)-QUARTERNOTE, tb->prevNTone(lastNote->getTono(),3));
			}
			else
			{
				if(step > 0)
					note = new Nota(durVertice.at(i)-QUARTERNOTE, tb->nextTone(lastNote->getTono()));
				else
					note = new Nota(durVertice.at(i)-QUARTERNOTE, tb->prevTone(lastNote->getTono()));
				seg->getSimbolos()->pushBack(note); //a�adimos la nota intermedia

				if(step > 0)
					note = new Nota(QUARTERNOTE, tb->nextNTone(lastNote->getTono(),3));
				else
					note = new Nota(QUARTERNOTE, tb->prevNTone(lastNote->getTono(),3));
			}
		}
		else
		{
			if(step > 0)
				note = new Nota(durVertice.at(i), tb->nextNTone(lastNote->getTono(), step));
			else
				note = new Nota(durVertice.at(i), tb->prevNTone(lastNote->getTono(), step));
		}

		if(note->getDuracion() > 0){
			seg->pushBack(note);  //La nota respecto al vertice.
		}
		lastNote = note;
	}
	lastNote = NULL;
	note = NULL;

	return true;
}

