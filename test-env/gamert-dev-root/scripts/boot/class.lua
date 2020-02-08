BtClass = function(ctor)
    local prototype = {}
    prototype.__index = prototype
    prototype.ctor = ctor
    prototype.super = nil

    setmetatable(prototype, {
        __call = function(...)
            local obj = {}
            setmetatable(obj, prototype)

            if prototype.ctor then
                prototype.ctor(obj, ...)
            end
            return obj
        end
    })

    return prototype
end

BtClassFrom = function(base, ctor)
    local prototype = BtClass(ctor)

    for key, value in pairs(base) do
        prototype[key] = value
    end

    prototype.super = base
    return prototype
end

