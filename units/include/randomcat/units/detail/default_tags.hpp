#pragma once

namespace randomcat::units::detail::default_tags {
    // Client code that directly uses these tags has undefined behavior
    
    struct time_tag {};
    struct distance_tag {};
    struct mass_tag {};
    struct temperature_tag {};
    struct current_tag {};
    struct luminous_intensity_tag {};
    struct angle_tag {};
}    // namespace randomcat::units::detail::default_tags