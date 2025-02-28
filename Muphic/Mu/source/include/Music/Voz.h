#pragma once

#ifndef VOZ_H
#define VOZ_H

#include "Music/Segmentos.h"
#include "Music/music_elements.h"

using namespace std;

class Voz
{
    public:

        /* Por ahora definimos los tipos como enteros */


        Voz();
        virtual ~Voz();

/*------Getters------*/
        int getInstrumento();
        Segmentos* getSegmentos();
		Tonalidad getTonalidad();

/*------Setters------*/
        void setInstrumento(int i);
        void setSegmentos(Segmentos* s);
		void setTonalidad(Tonalidad c);
		void setTempo(int t);

    protected:

    private:
        Instrumento instrumento;
		Tonalidad tonalidad;
		Segmentos* segmentos;
};

#endif // VOZ_H