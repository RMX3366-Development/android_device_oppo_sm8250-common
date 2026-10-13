/*
 * Copyright (C) 2022-2023 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/file.h>
#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/strings.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;
using android::base::ReadFileToString;
using android::base::Split;
using android::base::Trim;

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_load_properties() {
    auto prjname = std::stoi(GetProperty("ro.boot.prjname", "0"));

    switch (prjname) {
        // horee
        case 20061:
        case 20062:
            OverrideProperty("ro.product.product.model", "PEDM00");
            break;
        // hima
        case 20135:
        case 20139:
            OverrideProperty("ro.product.product.model", "PDRM00");
            break;
        case 20137:
            OverrideProperty("ro.product.product.model", "PDRT00");
            break;
        case 20235:
            OverrideProperty("ro.product.product.model", "CPH2207");
            break;
        // jin
        case 20161:
        case 20163:
            OverrideProperty("ro.product.product.model", "PENM00");
            break;
        case 20351:
            OverrideProperty("ro.product.product.model", "CPH2247");
            break;
        // rivena
        case 21615:
            OverrideProperty("ro.product.product.model", "RMX3366");
            break;
        default:
            LOG(ERROR) << "Unexpected project name: " << prjname;
    }

    if (std::string content; ReadFileToString("/proc/devinfo/ddr_type", &content)) {
        OverrideProperty("ro.boot.ddr_type", Split(Trim(content), "\t").back().c_str());
    }
}
