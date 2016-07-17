#ifndef CROSSFADER_H
#define CROSSFADER_H

#include <QtGlobal>
#include <thread>

class FaderThreadData;
class CrossFader
{

public:

    enum class FadeMode : quint8 {
	NoFading=0,
	FadeIn,
	FadeOut
    };

    CrossFader();

    virtual double get_current_volume() const =0;
    virtual void set_current_volume(double vol)=0;

    quint64 get_fade_overlap() const;
    void set_fade_overlap(quint64 overlap);

    quint64 get_fading_time() const;
    void set_fading_time(quint64 time);

    void fade_in();
    void fade_out();

    void fader_timed_out();


private:
    FadeMode	    _fade_mode;
    double	    _fade_step;
    quint64	    _fade_overlap;
    quint64	    _fading_time;
    std::thread*    _fader=nullptr;


    FaderThreadData * _fader_data=nullptr;


private:
    void increase_volume();
    void decrease_volume();

    void init_fader();


protected:

    void	    abort_fader();




};


#endif // CROSSFADER_H
