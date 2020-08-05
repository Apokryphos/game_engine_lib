#include <spine/Extension.h>

//  ----------------------------------------------------------------------------
spine::SpineExtension* spine::getDefaultExtension() {
    return new spine::DefaultSpineExtension();
}
