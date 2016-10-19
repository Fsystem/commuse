all: $(BUILDDIR)DNSAuthentication$(DLSUFFICS)

DNSAuthentication$(OBJSUFFICS): DNSAuthentication.c
	$(CC) $(DCFLAGS) $(CFLAGS) DNSAuthentication.c

$(BUILDDIR)DNSAuthentication$(DLSUFFICS): DNSAuthentication$(OBJSUFFICS)
	$(LN) $(LNOUT)../../$(BUILDDIR)DNSAuthentication$(DLSUFFICS) $(LDFLAGS) $(DLFLAGS) DNSAuthentication$(OBJSUFFICS) $(LIBS)

