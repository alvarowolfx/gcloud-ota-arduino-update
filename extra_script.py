Import("env")

# access to global construction environment
#print env

build_tag = env['PIOENV']
env.Replace(PROGNAME="firmware_%s" % build_tag)

# Dump construction environments (for debug purpose)
#print env.Dump()
