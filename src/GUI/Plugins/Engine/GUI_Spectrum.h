/* GUI_Spectrum.h */

/* Copyright (C) 2011-2017  Lucio Carreras
 *
 * This file is part of sayonara player
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GUI_SPECTRUM_H
#define GUI_SPECTRUM_H

#include "Utils/Pimpl.h"
#include "Components/Engine/Playback/SoundOutReceiver.h"

#include "EnginePlugin.h"

UI_FWD(GUI_Spectrum)

class GUI_Spectrum :
		public EnginePlugin,
		public SpectrumReceiver
{
    Q_OBJECT
	UI_CLASS(GUI_Spectrum)
    PIMPL(GUI_Spectrum)

public:
	explicit GUI_Spectrum(QWidget *parent=nullptr);
	virtual ~GUI_Spectrum();

	QString get_name() const override;
	QString get_display_name() const override;

protected:
	void paintEvent(QPaintEvent* e) override;
	void showEvent(QShowEvent*) override;
	void closeEvent(QCloseEvent*) override;
	void init_ui() override;
	void retranslate_ui() override;
    QWidget *widget() override;
    bool has_small_buttons() const override;

protected slots:
    void do_fadeout_step() override;

public slots:
	void set_spectrum(const SpectrumList& spec) override;
	void sl_update_style() override;

private:
    void resize_steps(int bins, int rects);

};

#endif // GUI_SPECTRUM_H
