/*
 * iluamaya.c : front-ent to interface ILU from Amaya.
 *
 * Daniel Veillard 1997
 *
 * To options are available :
 *    either Java support is also configured in, in this case
 *    the general main loop is handled by Kaffe threading support
 *    and the ILU support is just added to the glue between Amaya
 *    and Kaffe in ../javalib/kaffeine.c.
 *    Or ILU doesn't have to interract with kaffe, it just the
 *    matter of embedding ILU in an Xt/Motif application. In this
 *    case, ILU provides the network accesses (GET, POST, PUT) needed
 *    by amaya browser.
 */


#ifndef AMAYA_JAVA


#endif /* AMAYA_JAVA */
