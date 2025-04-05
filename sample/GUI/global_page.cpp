#include "global_page.h"

GlobalPage* GlobalPage::instance_ = 0;
GlobalPage* GlobalPage::Instance()
{
	if (0 == instance_) {
		instance_ = new GlobalPage();
	}
	return instance_;
}

void GlobalPage::Uninstance()
{
	if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

GlobalPage::GlobalPage()
{

}

GlobalPage::~GlobalPage()
{
	
}