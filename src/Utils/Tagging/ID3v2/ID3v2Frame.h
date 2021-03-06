/* AbstractFrame.h */

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

#ifndef ID3V2_FRAME_H_
#define ID3V2_FRAME_H_

#include "Utils/Tagging/AbstractFrame.h"

#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2frame.h>

#include <QString>

/**
 * @brief ID3v2Frame namespace
 * @ingroup Tagging
 */
namespace ID3v2
{
	template<typename ModelType_t, typename FrameType_t>
	/**
	 * @brief The AbstractFrame class\n
	 * for example
	 * AbstractFrame<Discnumber, TagLib::ID3v2::TextIdentificationFrame>
	 * @ingroup ID3v2
	 */
	class ID3v2Frame :
			protected Tagging::AbstractFrame<TagLib::ID3v2::Tag>
	{
		protected:
			FrameType_t*			_frame=nullptr;

		protected:

			/**
			 * @brief create_id3v2_frame creates new id3v2 frame
			 * if there's no frame we have to create it manually
			 * every subclass has to implement this function
			 * @return pointer to newly created frame
			 */
			virtual TagLib::ID3v2::Frame* create_id3v2_frame()=0;

			/**
			 * @brief map_model_to_frame\n
			 * maps the model to the frame and vice versa
			 * so the frame knows how to get/set data
			 */
			virtual void map_model_to_frame(const ModelType_t& model, FrameType_t* frame)=0;
			virtual void map_frame_to_model(const FrameType_t* frame, ModelType_t& model)=0;


		public:
			// constructor
			ID3v2Frame(const TagLib::FileRef& f, const char* four) :
				Tagging::AbstractFrame<TagLib::ID3v2::Tag>(nullptr, four)
			{
				TagLib::MPEG::File* mpeg = dynamic_cast<TagLib::MPEG::File*>(f.file());
				if(!mpeg){
					return;
				}

				TagLib::ID3v2::Tag* tag = mpeg->ID3v2Tag(true);
				this->set_tag(tag);
				if(!tag){
					return;
				}

				// map, containing [four][frame list]
				TagLib::ByteVector vec(four, 4);
				TagLib::ID3v2::FrameListMap map = tag->frameListMap();
				TagLib::ID3v2::FrameList frame_list = map[vec];
				if(!frame_list.isEmpty()) {
					_frame = dynamic_cast<FrameType_t*> (frame_list.front());
				}
			}

			// destructor
			virtual ~ID3v2Frame() {}


			//
			/**
			 * @brief sets the _data_model by reading from the frame
			 * @param data reference to data filled with _data_model
			 * @return false, if frame cannot be accessed, true else
			 */
			virtual bool read(ModelType_t& data)
			{
				if(!_frame){
					return false;
				}

				map_frame_to_model(_frame, data);

				return true;
			}


			//
			/**
			 * @brief insert the _data_model into the frame
			 * @param data_model the data model
			 * @return false if frame cannot be accessed
			 */
			virtual bool write(const ModelType_t& data_model)
			{
				bool created = false;

				TagLib::ID3v2::Tag* tag = this->tag();
				if(!tag){
					return false;
				}

				if(!_frame){
					_frame = dynamic_cast<FrameType_t*>(create_id3v2_frame());

					if(!_frame){
						return false;
					}
					created = true;
				}

				map_model_to_frame(data_model, _frame);

				if(created){
					// after that, no need to delete _frame
					tag->addFrame(_frame);
				}

				return true;
			}


			/**
			 * @brief if the frame was found when called read()
			 * @return true, if the frame was found
			 */
			bool is_frame_found() const 
			{
				return (_frame != nullptr);
			}
	};
}

#endif // ABSTRACTFRAME_H
