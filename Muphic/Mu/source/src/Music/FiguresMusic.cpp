#include "Music/FiguresMusic.h"

FiguresMusic::FiguresMusic()
{
    figuras.clear();
}

FiguresMusic::~FiguresMusic()
{
    figuras.clear();
}

Figura* FiguresMusic::createFigure()
{
	Figura* f = new FigureMusic();
	return f;
}

// recibe una lista de figuras con su vistosidad calculada y calcula su centro
pair<int,int> FiguresMusic::calcularCentro()
{
	FigureMusic* f;

	// calculamos la vistosidad total para normalizar
	float vistosidadTotal = 0;
	for (list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		f = (FigureMusic*) (*it);
		vistosidadTotal += f->getVistosidad();
	}

	// normalizamos la vistosidad de cada figura
	for (list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		f = (FigureMusic*) (*it);
		f->setVistosidad(f->getVistosidad()/vistosidadTotal);
	}

	
	// suponemos un pol�gono formado por los baricentros de cada figura, y calculamos su centro teniendo en cuenta la vistosidad
	pair<int,int> centerFigura;
	pair<int,int> centerTotal;
	centerTotal.first = 0;
	centerTotal.second = 0;
	for (list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		f = (FigureMusic*) (*it);
		centerFigura = f->getBarycenter();
		centerTotal.first += (int) f->getVistosidad() * centerFigura.first;
		centerTotal.second += (int) f->getVistosidad() * centerFigura.second;
	}
	
	return centerTotal;
}

void FiguresMusic::calculateVisibility()
{
	FigureMusic* f;

	// We calculate each figure visibility
	for(list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		f = (FigureMusic*) (*it);
		f->calcularVistosidad(sheetHeight, sheetWidth);
	}

	// We calculate the total visibility and the average RGB values
	float rM = 0;
	float gM = 0;
	float bM = 0;
	float vistosidadTotal;
	
	vistosidadTotal = 0;
	for(list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		f = (FigureMusic*) (*it);

		rM += f->getRGB().r;
		gM += f->getRGB().g;
		bM += f->getRGB().b;
		vistosidadTotal += f->getVistosidad();
	}

	rM = (float) rM / figuras.size();
	gM = (float) gM / figuras.size();
	bM = (float) bM / figuras.size();

	// We normalize figure visibility
	for(list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		f = (FigureMusic*) (*it);

		// Suma de las diferencias, en valor absoluto, de los valores rgb con los valores medios rgb de todas las figuras
		f->setColorDifference(abs(rM - f->getRGB().r) + abs(gM - f->getRGB().g) + abs(bM - f->getRGB().b));
		f->setVistosidad((f->getVistosidad() / vistosidadTotal));
	}


	figPadres = sortMusicFigures(figPadres);
	figuras = sortMusicFigures(figuras);
}

list<Figura*> FiguresMusic::sortMusicFigures(list<Figura*> figs)
{
	FigureMusic* f;
	list<FigureMusic*> aux;
	list<Figura*> aux2;

	for(list<Figura*>::iterator it = figs.begin(); it != figs.end(); it++)
	{
		f = (FigureMusic*) (*it);
		aux.push_back(f);
	}

	aux.sort(FigureMusic::compare);

	for(list<FigureMusic*>::iterator it = aux.begin(); it != aux.end(); it++)
	{
		(*it)->sortVertices();
		aux2.push_back((*it));
	}

	return aux2;
}