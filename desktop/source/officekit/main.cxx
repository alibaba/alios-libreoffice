#include <unistd.h>
#include "officekit.hxx"

int main()
{
    officekit::OfficeKit::Init("officeviewer.tv.yunos.com");

    officekit::OfficeKit::Main();


    return 0;
}
