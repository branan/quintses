#ifndef QNT_CORE_LOADERIFACE_H
#define QNT_CORE_LOADERIFACE_H

class LoaderIface {
public:
  virtual ~LoaderIface();

  virtual void finish() =0;
};

#endif // QNT_CORE_LOADERIFACE_H
