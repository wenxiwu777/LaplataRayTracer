#pragma once

#include <sys/time.h>
#include <iostream>
#include <iomanip>
#include <string>

using std::string;

namespace LaplataRayTracer
{
	//
	class IRenderWindowSink
	{
	public:
		virtual ~IRenderWindowSink() { }

	public:
        virtual void OnBeginRender(void *param) = 0;
		virtual void OnNotifyRenderProgress(int row, int total_row) = 0;
        virtual void OnEndRender(void *param) = 0;
        virtual string GetRenderReport(void *param) = 0;

	};

	//
    // The default implementation
    class RenderProcessReporter : public IRenderWindowSink {
    public:
        RenderProcessReporter() {
            mBeginTime = 0;
            mEndTime = 0;
        }

        virtual ~RenderProcessReporter() {

        }

    public:
        virtual void OnBeginRender(void *param) {
            std::cout << "begin to render" << std::endl;
            mBeginTime = get_current_time();
        }

        virtual void OnNotifyRenderProgress(int row, int total_row) {
            float per = (float)(row + 1) / (float)total_row;
            this->draw_progress_bar(per, '=');
            std::cout << std::fixed << std::setprecision(2) << "[" << 100.0f * per << "%" << "]" << std::endl;
        }

        virtual void OnEndRender(void *param) {
            mEndTime = get_current_time();
            std::cout << "rendering done!" << std::endl;
        }

        virtual string GetRenderReport(void *param) {
            long long delta_time = mEndTime - mBeginTime;
            std::cout << "elapsed time: " << delta_time << "ms" << " output file: " << *(string *)param << std::endl;
        }

    private:
        inline void draw_progress_bar(float per, char tag) {
            const int MAX_TAG = 40;
            std::cout << "[";
            int draw_count = MAX_TAG * per;
            for (int i = 0; i < draw_count; ++i) {
                std::cout << tag;
            }
            for (int i = draw_count; i < MAX_TAG; ++i) {
                std::cout << " ";
            }
            std::cout << "]";
        }

        inline long long get_current_time() {
            struct timeval tv;
            gettimeofday(&tv,NULL);
            return tv.tv_sec * 1000 + tv.tv_usec / 1000;
        }

    private:
        long long mBeginTime;
        long long mEndTime;

    };

}
