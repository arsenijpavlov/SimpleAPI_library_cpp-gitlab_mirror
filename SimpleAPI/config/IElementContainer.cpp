#include "IElementContainer.h"

#include "Config.h"


//не выносить в Header - требует включения "Config.h" (петля)
Config &IElementContainer::operator[](const size_t index) {
    return get_at(index);
}

//не выносить в Header - требует включения "Config.h" (петля)
Config IElementContainer::operator[](const size_t index) const {
    return get_at(index);
}
