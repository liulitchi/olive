/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2019  Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#ifndef CLIP_H
#define CLIP_H

#include <memory>
#include <QWaitCondition>
#include <QMutex>
#include <QVector>
#include <QOpenGLFramebufferObject>
#include <QOpenGLTexture>

#include "rendering/cacher.h"

#include "nodes/oldeffectnode.h"
#include "effects/transition.h"
#include "undo/comboaction.h"
#include "project/media.h"
#include "project/footage.h"
#include "rendering/framebufferobject.h"
#include "marker.h"
#include "nodes/nodegraph.h"
#include "selection.h"

class Track;

struct ClipSpeed {
  ClipSpeed();
  double value;
  bool maintain_audio_pitch;
};

class Clip {
public:
  Clip(Track *s);
  ~Clip();
  ClipPtr copy(Track *s);

  void Save(QXmlStreamWriter& stream);

  bool IsActiveAt(long timecode);
  bool IsSelected(bool containing = true);
  bool IsTransitionSelected(TransitionType type);

  Selection ToSelection();

  olive::TrackType type();

  const QColor& color();
  void set_color(int r, int g, int b);
  void set_color(const QColor& c);

  Media* media();
  FootageStream* media_stream();
  int media_stream_index();
  int media_width();
  int media_height();
  double media_frame_rate();
  long media_length();
  void set_media(Media* m, int s);

  void Move(ComboAction* ca,
            long iin,
            long iout,
            long iclip_in,
            Track *itrack,
            bool verify_transitions = true,
            bool relative = false);

  bool enabled();
  void set_enabled(bool e);

  long clip_in(bool with_transition = false);
  void set_clip_in(long c);

  long timeline_in(bool with_transition = false);
  void set_timeline_in(long t);

  long timeline_out(bool with_transition = false);
  void set_timeline_out(long t);

  Track* track();
  void set_track(Track* t);

  bool reversed();
  void set_reversed(bool r);

  bool autoscaled();
  void set_autoscaled(bool b);

  double cached_frame_rate();
  void set_cached_frame_rate(double d);

  const QString& name();
  void set_name(const QString& s);

  const ClipSpeed& speed();
  void set_speed(const ClipSpeed& s);

  AVRational time_base();

  void reset_audio();
  void refresh();

  long length();

  void refactor_frame_rate(ComboAction* ca, double multiplier, bool change_timeline_points);
  Track* parent_;

  // markers
  QVector<Marker>& get_markers();

  // other variables (should be deep copied/duplicated in copy())
  int IndexOfEffect(OldEffectNode* e);
  QList<OldEffectNodePtr> effects;
  QVector<Clip*> linked;
  TransitionPtr opening_transition;
  TransitionPtr closing_transition;

  // playback functions
  void Open();
  void Cache(long playhead, bool scrubbing, QVector<Clip*> &nests, int playback_speed);
  bool Retrieve();
  void Close(bool wait);
  bool IsOpen();

  bool UsesCacher();

  // temporary variables
  int load_id;
  bool undeletable;
  bool replaced;

  // caching functions
  QMutex state_change_lock;
  QMutex cache_lock;

  // video playback variables
  QVector<FramebufferObject> fbo;
  GLuint texture;
  int64_t texture_timestamp;

#ifndef NO_OCIO
  QOpenGLShaderProgramPtr ocio_shader;
  GLuint ocio_lut_texture;
#endif

private:
  // timeline variables (should be copied in copy())
  Track* track_;
  bool enabled_;
  long clip_in_;
  long timeline_in_;
  long timeline_out_;
  QString name_;
  Media* media_;
  int media_stream_;
  ClipSpeed speed_;
  double cached_fr_;
  bool reverse_;
  bool autoscale_;

  Cacher cacher;
  long cacher_frame;

  NodeGraph pipeline_;

  QVector<Marker> markers;
  QColor color_;
  bool open_;
};

#endif // CLIP_H
