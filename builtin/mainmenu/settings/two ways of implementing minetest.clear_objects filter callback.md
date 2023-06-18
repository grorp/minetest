
Depending on the use case, there are different ways to implement such a filter callback:

1. a callback within the luaentity table (if you want each entity to decide for itself whether it is deleted) OR

2. a callback passed as a parameter to `core.clear_objects` (if you want to implement filter rules like the ones proposed by this PR)

With variant 1, entities that want to be "safe" can always return `false` from the callback. By providing a default filter callback that checks for a specific luaentity field, variant 2 can also be used to provide a "safe mode".