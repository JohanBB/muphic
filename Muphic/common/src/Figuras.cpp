#include "Figuras.h"

Figuras::Figuras()
{
    figuras.clear();
}


Figuras::~Figuras()
{
    std::list<Figura*>::iterator it = figuras.begin();
    while (it != figuras.end())
    {
        delete (*it);
        it++;
    }
	figuras.clear();
}

// Funciones
void Figuras::cargar(string rutaXML)
{
	// Abrimos el archivo XML
	TiXmlDocument doc((rutaXML + ".xml").c_str());
	doc.LoadFile();

	// Leemos la cabecera
	TiXmlDeclaration* header = (TiXmlDeclaration*) doc.FirstChild();

	// Vamos a Shapes
	TiXmlNode* shapesNode = doc.FirstChild("shapes");

	//Cargamos el ancho y el alto de la p�gina
	sheetWidth = atoi(shapesNode->FirstChild("width")->ToElement()->GetText());
	sheetHeight = atoi(shapesNode->FirstChild("height")->ToElement()->GetText());

	//Reiniciamos el total de area y vertices
	totalArea = 0;
	totalVertices = 0;

	//Vamos a la primera figura
	TiXmlNode* figuraNode = shapesNode->FirstChild("figure");

	// Cargamos las figuras
	cargarRec(figuraNode,NULL);
}

void Figuras::guardar(string rutaXML)
{

	TiXmlDocument doc;

	// Header
	TiXmlDeclaration * headerNode = new TiXmlDeclaration( "1.0", "", "yes" );
	doc.LinkEndChild( headerNode );

	// First shapes node
	TiXmlElement * shapesNode = new TiXmlElement( "shapes" );
	doc.LinkEndChild( shapesNode );

	// Image size
	TiXmlElement * widthNode = new TiXmlElement( "width" );
	TiXmlElement * heightNode = new TiXmlElement( "height" );
	char size[10];
	our_itoa(this->sheetWidth, size, 10);
	widthNode->LinkEndChild( new TiXmlText(size)  );
	our_itoa(this->sheetHeight, size, 10);
	heightNode->LinkEndChild( new TiXmlText(size) );

	shapesNode->LinkEndChild( widthNode );
	shapesNode->LinkEndChild( heightNode );


	// Iterate father figures and add them to the xml doc
	TiXmlElement* figureNode;
	for (list<Figura*>::iterator it = figPadres.begin(); it != figPadres.end(); it++)
	{
		figureNode = new TiXmlElement("figure");
		guardarRec(figureNode, (*it));
		shapesNode->LinkEndChild( figureNode );
	}

	doc.SaveFile( rutaXML.c_str() );
}


void Figuras::guardarRec(TiXmlElement* f, Figura* id)
{

	// write id
	TiXmlElement * idNode = new TiXmlElement( "id" );
	char intstr[10];
	our_itoa(id->id, intstr, 10);
	idNode->LinkEndChild( new TiXmlText(intstr)  );
	f->LinkEndChild( idNode );

	// write color
	TiXmlElement * colorNode = new TiXmlElement( "color" );
		//name
		TiXmlElement * nameNode = new TiXmlElement( "name" );
		nameNode->LinkEndChild( new TiXmlText("Dovahkiin") );
		//rgb
		TiXmlElement * rgbNode = new TiXmlElement( "RGB" );
			// r
			TiXmlElement * rNode = new TiXmlElement( "R" );
			our_itoa(id->getRGB().r, intstr, 10);
			rNode->LinkEndChild( new TiXmlText(intstr) );
			//g
			TiXmlElement * gNode = new TiXmlElement( "G" );
			our_itoa(id->getRGB().g, intstr, 10);
			gNode->LinkEndChild( new TiXmlText(intstr) );
			// b
			TiXmlElement * bNode = new TiXmlElement( "B" );
			our_itoa(id->getRGB().b, intstr, 10);
			bNode->LinkEndChild( new TiXmlText(intstr) );
		rgbNode->LinkEndChild( rNode );
		rgbNode->LinkEndChild( gNode );
		rgbNode->LinkEndChild( bNode );
		// gradient
		TiXmlElement * gradientNode = new TiXmlElement( "gradient" );
		gradientNode->LinkEndChild( new TiXmlText("0") );
		// add all
		colorNode->LinkEndChild( nameNode );
		colorNode->LinkEndChild( rgbNode );
		colorNode->LinkEndChild( gradientNode);
	f->LinkEndChild( colorNode );


	// write vertex list
	TiXmlElement * vertexListNode = new TiXmlElement( "vertexList" );
		// size attribute
		our_itoa(id->listaVertices.size(), intstr, 10);
		vertexListNode->SetAttribute("num", intstr);
		// vertex nodes
		TiXmlElement * vertexNode;
		TiXmlElement * positionNode, * xNode, * yNode;
		for (list<Vertice*>::iterator it = id->listaVertices.begin(); it != id->listaVertices.end(); it++)
		{
			vertexNode = new TiXmlElement("vertex");
			//center vertex
			if ((*it)->centro)
			{
				vertexNode->SetAttribute("type", "center");
			}

			// position
			positionNode = new TiXmlElement("position");
				// x position
				xNode = new TiXmlElement("x");
				our_itoa((*it)->x, intstr, 10);
				xNode->LinkEndChild( new TiXmlText(intstr) );
				// y position
				yNode = new TiXmlElement("y");
				our_itoa((*it)->y, intstr, 10);
				yNode->LinkEndChild( new TiXmlText(intstr) );
				// add position
				positionNode->LinkEndChild( xNode );
				positionNode->LinkEndChild( yNode );
			// add info to node
			vertexNode->LinkEndChild( positionNode );
			vertexListNode->LinkEndChild( vertexNode );
		}
	f->LinkEndChild( vertexListNode );



	// write area
	TiXmlElement * areaNode = new TiXmlElement( "area" );
	our_itoa(id->area, intstr, 10);
	areaNode->LinkEndChild( new TiXmlText(intstr)  );
	f->LinkEndChild( areaNode );

	//  write children, if any
	if (id->hijos.size() != 0)
	{
		TiXmlElement * canvasNode = new TiXmlElement( "canvas" );
		TiXmlElement* figureNode;
		for (list<Figura*>::iterator it = id->hijos.begin(); it != id->hijos.end(); it++)
		{
			figureNode = new TiXmlElement("figure");
			guardarRec(figureNode, (*it));
			canvasNode->LinkEndChild( figureNode );
		}
		f->LinkEndChild( canvasNode );
	}
}

void Figuras::cargarRec(TiXmlNode* f, Figura* id)
{
	if(f != NULL)
	{
		// Creamos la nueva figura
		Figura* figura = createFigure();

		// Manipulamos el nodo xml
		TiXmlHandle handle(f);

		// Conseguimos los atributos de la figura
		figura->setId(atoi(handle.FirstChildElement("id").ToElement()->GetText()));
		int sizeVertices = atoi(handle.FirstChildElement("vertexList").ToElement()->Attribute("num"));
		figura->setParent(id);
		figura->setColor(handle.FirstChildElement("color").FirstChildElement("name").ToElement()->GetText());
		figura->setRGB((float)atof(handle.FirstChildElement("color").FirstChildElement("RGB").FirstChildElement("R").ToElement()->GetText()),
						 (float)atof(handle.FirstChildElement("color").FirstChildElement("RGB").FirstChildElement("G").ToElement()->GetText()),
						 (float)atof(handle.FirstChildElement("color").FirstChildElement("RGB").FirstChildElement("B").ToElement()->GetText()));
		// Preparamos el tratamiento de vertices
		Vertice* v;
		TiXmlNode* vertice = f->FirstChildElement("vertexList")->FirstChildElement("vertex");

		// Conseguimos la lista de v�rtices de la figura
		for( int i = 0; i < sizeVertices; i++)
		{
			v = new Vertice();
			v->x = atoi(vertice->FirstChildElement("position")->FirstChildElement("x")->GetText());
			v->y = sheetHeight - atoi(vertice->FirstChildElement("position")->FirstChildElement("y")->GetText());

			// Comprobamos que tenga atributo
			if(vertice->ToElement()->Attribute("type") != 0)
			{
				string l = vertice->ToElement()->Attribute("type");
				if(strcmp(vertice->ToElement()->Attribute("type"),"center") == 0)
					v->centro = true;
			}
			figura->colocarVertice(v);
			vertice = vertice->NextSibling("vertex");
		}

		// Leemos su area
		figura->setArea(atoi(handle.FirstChildElement("area").ToElement()->GetText()));

		// Tratamos los casos espec�ficos de figura hijo o figura padre
		if(id != NULL)
		{
			id->colocarHijo(figura);
		} // if id != NULL
		else
		{
			colocarPadre(figura);
		}

		// A�adimos la figura a la lista de figuras
		colocarFig(figura);

		//A�adimos al total de vertices y area
		totalVertices += figura->getNumVertices();
		totalArea += figura->getArea();

		// Si la figura tiene figuras en su interior hacemos la llamada recursiva pertinente
		if(handle.FirstChildElement("canvas").ToElement())
		{
			cargarRec(f->FirstChildElement("canvas")->FirstChildElement("figure"),figura);
		} // if canvas

		// Hacemos la llamada recursiva
		cargarRec(f->NextSibling("figure"),id);
	} // if f != NULL
}

// Envoltorio de list
bool Figuras::emptyFig()
{
	return figuras.empty();
}

bool Figuras::emptyPadre()
{
	return figPadres.empty();
}

int Figuras::sizeFig()
{
	return figuras.size();
}

int Figuras::sizePadre()
{
	return figPadres.size();
}

void Figuras::colocarFig(Figura* s)
{
	figuras.push_back(s);

	figuras.sort(&Figura::compare);
}

void Figuras::removeFig(Figura* f)
{
	figuras.remove(f);

	figuras.sort(&Figura::compare);
}

void Figuras::colocarPadre(Figura* s)
{
	figPadres.push_back(s);

	figPadres.sort(&Figura::compare);
}

//void Figuras::insertFig(Figura* s, int n)
//{
//	list<Figura*>::iterator it = figuras.begin();
//
//	for(int i = 0; i < n; i++)
//	{
//		it++;
//	}
//
//	figuras.insert(it,s);
//}
//
//void Figuras::insertPadre(Figura* s, int n)
//{
//	list<Figura*>::iterator it = figPadres.begin();
//
//	for(int i = 0; i < n; i++)
//	{
//		it++;
//	}
//
//	figPadres.insert(it,s);
//}

Figura* Figuras::getFigAt(int n)
{
	list<Figura*>::iterator it = figuras.begin();

	for(int i = 0; i < n; i++)
	{
		it++;
	}

	return *it;
}

Figura* Figuras::getPadreAt(int n)
{
	list<Figura*>::iterator it = figPadres.begin();

	for(int i = 0; i < n; i++)
	{
		it++;
	}

	return *it;
}

/*
// recibe una lista de figuras con su vistosidad calculada y calcula su centro
pair<int,int> Figuras::calcularCentro()
{
	// calculamos la vistosidad total para normalizar
	float vistosidadTotal = 0;
	for (list<FigureMusic*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		vistosidadTotal += (*it)->getVistosidad();
	}

	// normalizamos la vistosidad de cada figura
	for (list<FigureMusic*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		(*it)->vistosidad /= vistosidadTotal;
	}


	// suponemos un pol�gono formado por los baricentros de cada figura, y calculamos su centro teniendo en cuenta la vistosidad
	pair<int,int> centerFigura;
	pair<int,int> centerTotal;
	centerTotal.first = 0;
	centerTotal.second = 0;
	for (list<FigureMusic*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{
		centerFigura = (*it)->getBarycenter();
		centerTotal.first += (int)(*it)->vistosidad * centerFigura.first;
		centerTotal.second += (int)(*it)->vistosidad * centerFigura.second;
	}

	return centerTotal;
}
*/

/*------Getters------*/
int Figuras::getHeight()
{
	return sheetHeight;
}

int Figuras::getWidth()
{
	return sheetWidth;
}

int Figuras::getTotalVertices()
{
	return totalVertices;
}

int Figuras::getTotalArea()
{
	return totalArea;
}

list<Figura*> Figuras::getPadres()
{
    return figPadres;
}

//list<Figura*>* Figuras::getFiguras()
//{
//	return &figuras;
//}
//
//list<Figura*>* Figuras::getFigPadre()
//{
//	return &figPadres;
//}

Figura* Figuras::createFigure()
{
	Figura* f = new Figura();
	return f;
}

void Figuras::renameStructure(list<Figura*> padres, Figura* padre)
{
	list<Figura*>::iterator it = padres.begin();
	list<Figura*>::iterator jt;
	for (it; it != padres.end(); it++)
	{
		jt = it; jt++;

		(*it)->padre = padre;

		if (jt != padres.end())
			(*it)->nextSibling = *jt;
		else
			(*it)->nextSibling = NULL;

		jt = it;
		if (jt == padres.begin())
			(*it)->prevSibling = NULL;
		else
		{
			jt--;
			(*it)->prevSibling = *jt;
		}

		renameStructure((*it)->hijos, *it);
	}
}

void Figuras::showParentSonStructure(list<Figura*> padres, int level)
{
	for (list<Figura*>::iterator it = padres.begin(); it != padres.end(); it++)
	{
		for (int i = 0; i < level; i++)
		{
			cout << "  ";
		}
		cout << (*it)->sizeVertices() << endl;
		showParentSonStructure((*it)->hijos, level + 1);
	}
}


void Figuras::addToParentSonStructure(Figura* f, list<Figura*> & padres)
{
	list<Figura*> deletables;
	list<Figura*>::iterator it = padres.begin();
	bool done = false;
	while (it != padres.end() && !done)
	{
		// case 1: f is inside it
		if ((*it)->isFigureInside(f))
		{
			// f may be the son of one of it's children
			addToParentSonStructure(f, (*it)->hijos);
			// f is inside this figure and we have finished
			done = true;
		}
		// case 2: it is inside f
		else if (f->isFigureInside(*it))
		{
			// it is f's son
			f->colocarHijo(*it);
			// it does not belong to this level anymore, we'll delete it later
			deletables.push_back(*it);
			// f may contain more figures, so duh
			it++;
		}
		// none of that happens
		else
		{
			it++;
		}
	}

	// let's delete those duplicates
	it = deletables.begin();
	while (it != deletables.end())
	{
		padres.remove(*it);
		it++;
	}

	// if noone contains f, it means f belongs to this level
	if (!done)
	{
		padres.push_back(f);
	}
}


void Figuras::setParentSonStructure()
{
	// if figPadres is not empty, delete it
	if (figPadres.size() != 0)
	{
		figPadres.clear();

		for (list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
		{
			(*it)->hijos.clear();
		}
	}
	

	list<Figura*>::iterator it = figuras.begin();

	// first figure is the initial solution
	figPadres.push_back(*it);
	it++;
	for (it; it != figuras.end(); it++)
	{
		addToParentSonStructure((*it), figPadres);
	}

	renameStructure(figPadres, NULL);
	showParentSonStructure(figPadres, 0);
}


bool** Figuras::fillMask(Figura* f)
{
	bool ** mask = new bool*[f->yT - f->yB + 1];

	int nodes, *pixelX;
	int nvertex = f->sizeVertices();
	pixelX = new int[nvertex];
	Vertice* currentvertex, * lastvertex;
	list<Vertice*>::iterator it, jt;
	int i, swap;
	for (int pixelY = f->yB; pixelY < f->yT; pixelY++)
	{
		//  Build a list of nodes.
		nodes = 0;
		jt = f->listaVertices.end(); jt--;
		for (it = f->listaVertices.begin(); it != f->listaVertices.end(); it++)
		{
			currentvertex = *it;
			lastvertex = *jt;

			if (currentvertex->y < (double) pixelY && lastvertex->y >= (double) pixelY 
				|| lastvertex->y < (double) pixelY && currentvertex->y >= (double) pixelY)
			{
				pixelX[nodes++] = (int) (currentvertex->x + (pixelY - currentvertex->y) * (lastvertex->x - currentvertex->x) / (lastvertex->y - currentvertex->y));
			}

			jt = it;
		}

		//  Sort the nodes, via a simple �Bubble� sort.
		i=0;
		while (i < nodes-1) 
		{
			if (pixelX[i] > pixelX[i+1]) 
			{
				swap = pixelX[i]; 
				pixelX[i] = pixelX[i+1]; 
				pixelX[i+1] = swap; 
				if (i)
					i--; 
			}
			else 
			{
				i++; 
			}
		}


		mask[pixelY - f->yB] = new bool[f->xR - f->xL + 1];
		for (int j = 0; j < f->xR - f->xL + 1; j++)
			mask[pixelY - f->yB][j] = false;

		//  Fill the pixels between node pairs.		
		for (i = 0; i < nodes; i += 2)
		{
			if (pixelX[i] >= f->xR)
				break;
			if (pixelX[i+1] > f->xL)
			{
				if (pixelX[i] < f->xL)
					pixelX[i] = f->xL;
				if (pixelX[i+1] > f->xR)
					pixelX[i+1] = f->xR;
				for (int j = pixelX[i]; j < pixelX[i+1]; j++)
					mask[pixelY - f->yB][j - f->xL] = true;
					
			}
		
		}

	}

	return mask;
}

#include <fstream>

bool Figuras::lcAreSimilar(Figura* a, Figura* b, double eps)
{
	Figura *main, *sub;

	if (a->area > b->area)
	{
		main = a;
		sub = b;
	}
	else
	{
		main = b;
		sub = a;
	}

	bool** maskM = fillMask(main);
	bool** maskS = fillMask(sub);

	bool cellM;
	bool cellS;
	int commonpixels = 0;
	for (int i = 0; i < max(main->yT - main->yB, sub->yT - sub->yB) ; i++)
	{
		for (int j = 0; j < max(main->xR - main->xL, sub->xR - sub->xL); j++)
		{	
			if (i >= (main->yT - main->yB) || j >= (main->xR - main->xL))
				cellM = false;
			else
				cellM = maskM[i][j];

			if (i >= (sub->yT - sub->yB) || j >= (sub->xR - sub->xL))
				cellS = false;
			else
				cellS = maskS[i][j];

			if (cellM && cellS)
				commonpixels++;
		}
	}

	if ((commonpixels - main->area) < 0)
		commonpixels = main->area - commonpixels;
	else
		commonpixels = commonpixels - main->area;

	return commonpixels < eps;
}

bool Figuras::fcAreSimilar(Figura* a, Figura* b, double eps)
{
	Figura* main, * sub;
	if (a->area > b->area)
	{
		main = a;
		sub = b;
	}
	else
	{
		main = b;
		sub = a;
	}


	double dL = a->xL - b->xL; double dR = a->xR - b->xR;
	double dT = a->yT - b->yT; double dB = a->yB - b->yB;

	if (dL < 0) dL = -dL; if (dR < 0) dR = -dR;
	if (dT < 0) dT = -dT; if (dB < 0) dB = -dB;

	double sum = (dL + dR)*(main->yT - main->yB) + (dT + dB)*(main->xR - main->xL);

	double dred = a->rgb.r - b->rgb.r;
	double dgreen = a->rgb.g - b->rgb.g;
	double dblue = a->rgb.b - b->rgb.b;

	if (dred < 0) dred = -dred;
	if (dgreen < 0) dgreen = -dgreen;
	if (dblue < 0) dblue = -dblue; 

	double colorcheck = dred + dgreen + dblue;

	return (sum < eps);// && (colorcheck < 20);
}

void Figuras::showFig(Figura* f)
{
	ofstream myfile;
	myfile.open ("te.txt");
	bool** maskM = fillMask(f);

	for (int i = 0; i < (f)->yT - (f)->yB; i++)
	{
		for (int j = 0; j < (f)->xR - (f)->xL; j++)
			if (maskM[i][j])
				myfile << "1" << " ";
			else
				myfile << " " << " ";
		myfile << "|" << endl;
	}	

	myfile.close();
}

void Figuras::showFigs()
{
	for (list<Figura*>::iterator it = figuras.begin(); it != figuras.end(); it++)
	{

		ofstream myfile;
		myfile.open ("te.txt");
		bool** maskM = fillMask(*it);
		myfile << (*it)->xL << " " << (*it)->xR;
		myfile << (*it)->yT << " " << (*it)->yB;

		for (int i = 0; i < (*it)->yT - (*it)->yB; i++)
		{
			for (int j = 0; j < (*it)->xR - (*it)->xL; j++)
				if (maskM[i][j])
					myfile << "1" << " ";
				else
					myfile << " " << " ";
			myfile << "|" << endl;
		}	

		myfile.close();
	}
}

void Figuras::deleteReps()
{
	// fast-check
	set<Figura*> deletables;
	list<Figura*>::iterator it;
	list<Figura*>::iterator limit = figuras.end();
	limit--;
	list<Figura*>::iterator jt;
	int i = 0;
	double eps;
	double aprox = 15;
	int area1, area2;
	for (it = figuras.begin(); it != limit; it++)
	{
		jt = it; jt++;
		for (jt; jt != figuras.end(); jt++)
		{
			// eps is aprox% of max area
			area1 = (*it)->area;
			area2 = (*jt)->area;
			//showFig(*it);
			//showFig(*jt);
			eps = aprox * max((*it)->area, (*jt)->area) / 100;

			if (fcAreSimilar(*it, *jt, 2*eps))
			{
				if (lcAreSimilar(*it, *jt, eps))
				{
					deletables.insert(*it);
				}
			}
		}
		i++;
	}


	// let's delete those duplicates
	set<Figura*>::iterator dt;
	dt = deletables.begin();
	while (dt != deletables.end())
	{
		figuras.remove(*dt);
		delete *dt;
		dt++;
	}

}

void Figuras::redoColorFig(Figura* f)
{
	if (f->hijos.size() > 0)
	{
		double racum = 0;
		double gacum = 0;
		double bacum = 0;
		double p;
		double rarea = f->area;
		for (list<Figura*>::iterator it = f->hijos.begin(); it != f->hijos.end(); it++)
		{
			rarea -= (*it)->area;
			// prop. area
			p = (*it)->area / (double) f->area;
			racum += p*(*it)->rgb.r;
			gacum += p*(*it)->rgb.g;
			bacum += p*(*it)->rgb.b;
		}

		double rfinal = (double) f->rgb.r - racum;
		double gfinal = (double) f->rgb.g - gacum;
		double bfinal = (double) f->rgb.b - bacum;

		f->rgb.r = rfinal * f->area / (double) rarea;
		f->rgb.g = gfinal * f->area / (double) rarea;
		f->rgb.b = bfinal * f->area / (double) rarea;
	}
}

void Figuras::redoColorFigs()
{
	if (figPadres.size() > 0)
	{
		Figura* currentson = *(figPadres.begin());
		while (currentson->getFirstSon() != 0) currentson = currentson->getFirstSon();

		while (currentson != NULL)
		{
			// do shit
		
			redoColorFig(currentson);

			if (currentson->getNextSibling())
			{
				currentson = currentson->getNextSibling();
				// down till the end
				while (currentson->getFirstSon() != 0) currentson = currentson->getFirstSon();				
			}
			else
				currentson = currentson->getParent();
		}
	}
}