#ifndef _NULLSPACE_ANSYCWORKER_1590248311419_H
#define _NULLSPACE_ANSYCWORKER_1590248311419_H
#include "qtusercore/module/job.h"
#include <QtCore/QThread>
#include "us/usettings.h"
#include "qtusercore/module/formattracer.h"
#include "crslice2/crscene.h"

namespace creative_kernel
{
	class SliceAttainTracer : public qtuser_core::FormatTracer
	{
		Q_OBJECT
	public:
		SliceAttainTracer(qtuser_core::Progressor* progressor);
		virtual ~SliceAttainTracer() {}

		int indexCount() override;

		QString indexStr(int index, va_list vars) override;
	};

	class FormatSlice : public qtuser_core::FormatTracer
	{
		Q_OBJECT
	public:
		FormatSlice(qtuser_core::Progressor* progressor);
		virtual ~FormatSlice() {}

		int indexCount() override;

		QString indexStr(int index, va_list vars);
	};

	class Crslice2Tracer : public qtuser_core::ProgressorTracer
	{
	public:
		Crslice2Tracer(qtuser_core::Progressor* progressor, QObject* parent = nullptr);
		virtual ~Crslice2Tracer();

		void message(const char* msg) override;
	};

	class SliceAttain;
	class Printer;
	class ModelN;
	class AnsycWorker : public qtuser_core::Job
	{
		Q_OBJECT
	public:
		AnsycWorker(QObject* parent = nullptr);
		virtual ~AnsycWorker();
		SliceAttain* sliceAttain();
		void setRemainAttain(SliceAttain* attain);
		void setSlicePrinter(Printer* printer);
		Printer* slicePrinter();
		void applyAdditionalSliceParameter();

		void waitForCompleted();

		void cancel();

	protected:
		QString name() override;
		QString description() override;
		void failed() override;
		void successed(qtuser_core::Progressor* progressor) override;
		void work(qtuser_core::Progressor* progressor) override;

		bool needReSlice();

		void prepareSliceData();

	signals:
		void prepareSlice(Printer* printer);
		void sliceMessage(const QString& message);
		void sliceSuccess(SliceAttain* attain, Printer* printer);
		void sliceBeforeSuccess(SliceAttain* attain);
		void sliceFailed(const QString& failReason);

	signals:
		void gcodeLayerChanged(SliceAttain* attain, int layer);

	protected:
		SliceAttain* m_sliceAttain{ NULL };
		SliceAttain* m_remainAttain{ NULL };
		Printer* m_printer;
        bool m_slicing { true };

		QList<ModelN*> m_slicedModels;
		std::vector<crslice2::ThumbnailData> m_thumbnailDatas;
		crslice2::plateInfo m_plateInfo;

		QString m_failExceptionDesc;
	
	};
}
#endif // _NULLSPACE_ANSYCWORKER_1590248311419_H
