/*
 *	PFE Simulateur QSx86 2011-2012
 *	Logiciel EVE
 *
 *	Fichier : interface.c
 *	Package : EVE
 *	Description : Fichier de déclaration des éléments composant l'interface
 *	Auteur : Julien Franchineau & François Even
 *	Version 20111228
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "IHM_callbacks.h"
#include "IHM_interface.h"
#include "IHM_support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_Fenetre (void)
{
  GtkWidget *Fenetre;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *menuitem10;
  GtkWidget *menu10;
  GtkWidget *nouveau1;
  GtkWidget *ouvrir1;
  GtkWidget *enregistrer1;
  GtkWidget *enregistrer_sous1;
  GtkWidget *separatormenuitem1;
  GtkWidget *quitter1;
  GtkWidget *menuitem11;
  GtkWidget *menu11;
  GtkWidget *couper1;
  GtkWidget *copier1;
  GtkWidget *coller1;
  GtkWidget *supprimer1;
  GtkWidget *menuitem12;
  GtkWidget *menu12;
  GtkWidget *menuitem13;
  GtkWidget *menu13;
  GtkWidget *___propos1;
  GtkWidget *hbox1;
  GtkWidget *vbox7;
  GtkWidget *vbox8;
  GtkWidget *hbox2;
  GtkWidget *label14;
  GtkWidget *can_button_clear;
  GtkWidget *vbox9;
  GtkWidget *scrolledwindow2;
  GtkWidget *can_zt_main;
  GtkWidget *notebook2;
  GtkWidget *scrolledwindow11;
  GtkWidget *viewport1;
  GtkWidget *vbox27;
  GtkWidget *table5;
  GtkWidget *label34;
  GtkWidget *label35;
  AtkObject *atko;
  GtkWidget *scrolledwindow12;
  GtkWidget *viewport2;
  GtkWidget *table6;
  GtkWidget *can_msg_button_bt1;
  GtkWidget *button32;
  GtkWidget *button33;
  GtkWidget *scrolledwindow13;
  GtkWidget *can_msg_zt_desc1;
  GtkWidget *scrolledwindow14;
  GtkWidget *can_msg_zt_msg1;
  GtkWidget *scrolledwindow15;
  GtkWidget *textview11;
  GtkWidget *scrolledwindow16;
  GtkWidget *textview12;
  GtkWidget *scrolledwindow17;
  GtkWidget *textview13;
  GtkWidget *scrolledwindow18;
  GtkWidget *textview14;
  GtkWidget *label15;
  GtkWidget *vbox28;
  GtkWidget *can_send_st_main;
  GtkWidget *hbox9;
  GtkWidget *can_send_button_add;
  GtkWidget *can_send_button_send;
  GtkWidget *label10;
  GtkWidget *vbox41;
  GtkWidget *frame13;
  GtkWidget *alignment28;
  GtkWidget *table41;
  GtkWidget *can_filt_chk_all;
  GtkWidget *can_filt_chk_bal;
  GtkWidget *can_filt_chk_sup;
  GtkWidget *can_filt_chk_stra;
  GtkWidget *can_filt_chk_pro;
  GtkWidget *can_filt_chk_act;
  GtkWidget *label48;
  GtkWidget *frame14;
  GtkWidget *alignment29;
  GtkWidget *hbox19;
  GtkWidget *frame15;
  GtkWidget *alignment30;
  GtkWidget *can_filt_st_sid;
  GtkWidget *label50;
  GtkWidget *frame16;
  GtkWidget *alignment31;
  GtkWidget *can_filt_st_data;
  GtkWidget *label51;
  GtkWidget *frame17;
  GtkWidget *alignment32;
  GtkWidget *can_filt_st_taille;
  GtkWidget *label52;
  GtkWidget *can_filt_button_ok;
  GtkWidget *image20;
  GtkWidget *label49;
  GtkWidget *label11;
  GtkWidget *hbox3;
  GtkWidget *vbox11;
  GtkWidget *toolbar20;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *toolitem104;
  GtkWidget *os_button_play;
  GtkWidget *alignment300;
  GtkWidget *hbox191;
  GtkWidget *image108;
  GtkWidget *label339;
  GtkWidget *toolitem105;
  GtkWidget *os_button_pause;
  GtkWidget *alignment301;
  GtkWidget *hbox192;
  GtkWidget *image109;
  GtkWidget *label340;
  GtkWidget *toolitem106;
  GtkWidget *os_button_stop;
  GtkWidget *image110;
  GtkWidget *toolitem107;
  GtkWidget *frame199;
  GtkWidget *alignment302;
  GtkWidget *hbox193;
  GtkWidget *os_st_timeJump;
  GtkWidget *os_button_jumpOk;
  GtkWidget *label341;
  GtkWidget *toolitem108;
  GtkWidget *frame200;
  GtkWidget *alignment303;
  GtkWidget *scrolledwindow114;
  GtkWidget *os_zt_CurrentTime;
  GtkWidget *label342;
  GtkWidget *toolitem109;
  GtkWidget *frame202;
  GtkWidget *alignment305;
  GtkWidget *scrolledwindow116;
  GtkWidget *os_zt_matchTime;
  GtkWidget *label350;
  GtkWidget *cei_ong_all;
  GtkWidget *vbox102;
  GtkWidget *table30;
  GtkWidget *scrolledwindow144;
  GtkWidget *viewport7;
  GtkWidget *vbox103;
  GtkWidget *hbox269;
  GtkWidget *hbox270;
  GtkWidget *label11199;
  GtkWidget *image165;
  GtkWidget *hbox271;
  GtkWidget *label11200;
  GtkWidget *image166;
  GtkWidget *hbox272;
  GtkWidget *label11201;
  GtkWidget *image167;
  GtkWidget *hbox273;
  GtkWidget *label11202;
  GtkWidget *image168;
  GtkWidget *hbox274;
  GtkWidget *label11203;
  GtkWidget *image169;
  GtkWidget *hbox275;
  GtkWidget *label11204;
  GtkWidget *image170;
  GtkWidget *frame251;
  GtkWidget *alignment369;
  GtkWidget *image171;
  GtkWidget *label11205;
  GtkWidget *hbuttonbox5;
  GtkWidget *cei_act_button_bt1;
  GtkWidget *cei_act_button_bt2;
  GtkWidget *cei_act_button_bt3;
  GtkWidget *cei_act_button_bt4;
  GtkWidget *cei_act_button_reset;
  GtkWidget *image172;
  GtkWidget *vbox104;
  GtkWidget *table31;
  GtkWidget *vbox105;
  GtkWidget *frame252;
  GtkWidget *alignment370;
  GtkWidget *hbox276;
  GtkWidget *label11206;
  GtkWidget *scrolledwindow145;
  GtkWidget *textview107;
  GtkWidget *label11207;
  GtkWidget *vbox106;
  GtkWidget *frame253;
  GtkWidget *alignment371;
  GtkWidget *alignment372;
  GtkWidget *hbox277;
  GtkWidget *label11208;
  GtkWidget *scrolledwindow146;
  GtkWidget *textview108;
  GtkWidget *frame254;
  GtkWidget *alignment373;
  GtkWidget *hbox278;
  GtkWidget *entry64;
  GtkWidget *button144;
  GtkWidget *image173;
  GtkWidget *label11209;
  GtkWidget *label11210;
  GtkWidget *frame255;
  GtkWidget *alignment374;
  GtkWidget *alignment375;
  GtkWidget *hbox279;
  GtkWidget *label11211;
  GtkWidget *scrolledwindow147;
  GtkWidget *textview109;
  GtkWidget *label11212;
  GtkWidget *frame256;
  GtkWidget *alignment376;
  GtkWidget *hbox280;
  GtkWidget *label11213;
  GtkWidget *scrolledwindow148;
  GtkWidget *textview110;
  GtkWidget *frame257;
  GtkWidget *alignment377;
  GtkWidget *hbox281;
  GtkWidget *entry65;
  GtkWidget *button145;
  GtkWidget *image174;
  GtkWidget *label11214;
  GtkWidget *label11215;
  GtkWidget *frame258;
  GtkWidget *alignment378;
  GtkWidget *alignment379;
  GtkWidget *hbox282;
  GtkWidget *label11216;
  GtkWidget *scrolledwindow149;
  GtkWidget *textview111;
  GtkWidget *frame259;
  GtkWidget *alignment380;
  GtkWidget *hbox283;
  GtkWidget *entry66;
  GtkWidget *button146;
  GtkWidget *image175;
  GtkWidget *label11217;
  GtkWidget *label11218;
  GtkWidget *frame260;
  GtkWidget *alignment381;
  GtkWidget *drawingarea10;
  GtkWidget *label11219;
  GtkWidget *frame261;
  GtkWidget *alignment382;
  GtkWidget *alignment383;
  GtkWidget *hbox284;
  GtkWidget *label11220;
  GtkWidget *scrolledwindow150;
  GtkWidget *textview112;
  GtkWidget *frame262;
  GtkWidget *alignment384;
  GtkWidget *hbox285;
  GtkWidget *entry67;
  GtkWidget *button147;
  GtkWidget *image176;
  GtkWidget *label11221;
  GtkWidget *label11222;
  GtkWidget *frame263;
  GtkWidget *alignment385;
  GtkWidget *hbox286;
  GtkWidget *frame264;
  GtkWidget *alignment386;
  GtkWidget *scrolledwindow151;
  GtkWidget *textview113;
  GtkWidget *label11223;
  GtkWidget *frame265;
  GtkWidget *alignment387;
  GtkWidget *hbox287;
  GtkWidget *entry68;
  GtkWidget *button148;
  GtkWidget *image177;
  GtkWidget *label11224;
  GtkWidget *label11225;
  GtkWidget *frame266;
  GtkWidget *alignment388;
  GtkWidget *alignment389;
  GtkWidget *hbox288;
  GtkWidget *label11226;
  GtkWidget *scrolledwindow152;
  GtkWidget *textview114;
  GtkWidget *label11227;
  GtkWidget *label333;
  GtkWidget *vbox110;
  GtkWidget *table33;
  GtkWidget *scrolledwindow155;
  GtkWidget *viewport8;
  GtkWidget *vbox111;
  GtkWidget *hbox307;
  GtkWidget *hbox308;
  GtkWidget *label11256;
  GtkWidget *image186;
  GtkWidget *hbox309;
  GtkWidget *label11257;
  GtkWidget *image187;
  GtkWidget *hbox310;
  GtkWidget *label11258;
  GtkWidget *image188;
  GtkWidget *hbox311;
  GtkWidget *label11259;
  GtkWidget *image189;
  GtkWidget *hbox312;
  GtkWidget *label11260;
  GtkWidget *image190;
  GtkWidget *hbox313;
  GtkWidget *label11261;
  GtkWidget *image191;
  GtkWidget *frame281;
  GtkWidget *alignment410;
  GtkWidget *image192;
  GtkWidget *label11262;
  GtkWidget *hbuttonbox6;
  GtkWidget *cei_pro_button_bt1;
  GtkWidget *cei_pro_button_bt2;
  GtkWidget *cei_pro_button_bt3;
  GtkWidget *cei_pro_button_bt4;
  GtkWidget *button158;
  GtkWidget *image193;
  GtkWidget *vbox112;
  GtkWidget *table34;
  GtkWidget *vbox113;
  GtkWidget *frame282;
  GtkWidget *alignment411;
  GtkWidget *hbox314;
  GtkWidget *label11263;
  GtkWidget *scrolledwindow156;
  GtkWidget *textview116;
  GtkWidget *label11264;
  GtkWidget *vbox114;
  GtkWidget *frame283;
  GtkWidget *alignment412;
  GtkWidget *alignment413;
  GtkWidget *hbox315;
  GtkWidget *label11265;
  GtkWidget *scrolledwindow157;
  GtkWidget *textview117;
  GtkWidget *frame284;
  GtkWidget *alignment414;
  GtkWidget *hbox316;
  GtkWidget *entry69;
  GtkWidget *button159;
  GtkWidget *image194;
  GtkWidget *label11266;
  GtkWidget *label11267;
  GtkWidget *frame285;
  GtkWidget *alignment415;
  GtkWidget *alignment416;
  GtkWidget *hbox317;
  GtkWidget *label11268;
  GtkWidget *scrolledwindow158;
  GtkWidget *textview118;
  GtkWidget *label11269;
  GtkWidget *frame286;
  GtkWidget *alignment417;
  GtkWidget *hbox318;
  GtkWidget *label11270;
  GtkWidget *scrolledwindow159;
  GtkWidget *textview119;
  GtkWidget *frame287;
  GtkWidget *alignment418;
  GtkWidget *hbox319;
  GtkWidget *entry70;
  GtkWidget *button160;
  GtkWidget *image195;
  GtkWidget *label11271;
  GtkWidget *label11272;
  GtkWidget *frame288;
  GtkWidget *alignment419;
  GtkWidget *alignment420;
  GtkWidget *hbox320;
  GtkWidget *label11273;
  GtkWidget *scrolledwindow160;
  GtkWidget *textview120;
  GtkWidget *frame289;
  GtkWidget *alignment421;
  GtkWidget *hbox321;
  GtkWidget *entry71;
  GtkWidget *button161;
  GtkWidget *image196;
  GtkWidget *label11274;
  GtkWidget *label11275;
  GtkWidget *frame290;
  GtkWidget *alignment422;
  GtkWidget *drawingarea11;
  GtkWidget *label11276;
  GtkWidget *frame291;
  GtkWidget *alignment423;
  GtkWidget *alignment424;
  GtkWidget *hbox322;
  GtkWidget *label11277;
  GtkWidget *scrolledwindow161;
  GtkWidget *textview121;
  GtkWidget *frame292;
  GtkWidget *alignment425;
  GtkWidget *hbox323;
  GtkWidget *entry72;
  GtkWidget *button162;
  GtkWidget *image197;
  GtkWidget *label11278;
  GtkWidget *label11279;
  GtkWidget *frame293;
  GtkWidget *alignment426;
  GtkWidget *hbox324;
  GtkWidget *frame294;
  GtkWidget *alignment427;
  GtkWidget *scrolledwindow162;
  GtkWidget *textview122;
  GtkWidget *label11280;
  GtkWidget *frame295;
  GtkWidget *alignment428;
  GtkWidget *hbox325;
  GtkWidget *entry73;
  GtkWidget *button163;
  GtkWidget *image198;
  GtkWidget *label11281;
  GtkWidget *label11282;
  GtkWidget *frame296;
  GtkWidget *alignment429;
  GtkWidget *alignment430;
  GtkWidget *hbox326;
  GtkWidget *label11283;
  GtkWidget *scrolledwindow163;
  GtkWidget *textview123;
  GtkWidget *label11284;
  GtkWidget *label12333;
  GtkWidget *vbox115;
  GtkWidget *table35;
  GtkWidget *scrolledwindow164;
  GtkWidget *viewport9;
  GtkWidget *vbox116;
  GtkWidget *hbox327;
  GtkWidget *hbox328;
  GtkWidget *label11285;
  GtkWidget *image199;
  GtkWidget *hbox329;
  GtkWidget *label11286;
  GtkWidget *image200;
  GtkWidget *hbox330;
  GtkWidget *label11287;
  GtkWidget *image201;
  GtkWidget *hbox331;
  GtkWidget *label11288;
  GtkWidget *image202;
  GtkWidget *hbox332;
  GtkWidget *label11289;
  GtkWidget *image203;
  GtkWidget *hbox333;
  GtkWidget *label11290;
  GtkWidget *image204;
  GtkWidget *frame297;
  GtkWidget *alignment431;
  GtkWidget *image205;
  GtkWidget *label11291;
  GtkWidget *hbuttonbox7;
  GtkWidget *cei_stra_button_bt1;
  GtkWidget *button165;
  GtkWidget *cei_stra_button_bt3;
  GtkWidget *cei_stra_button_bt4;
  GtkWidget *cei_stra_button_reset;
  GtkWidget *image206;
  GtkWidget *vbox117;
  GtkWidget *table36;
  GtkWidget *vbox118;
  GtkWidget *frame298;
  GtkWidget *alignment432;
  GtkWidget *hbox334;
  GtkWidget *label11292;
  GtkWidget *scrolledwindow165;
  GtkWidget *textview124;
  GtkWidget *label11293;
  GtkWidget *vbox119;
  GtkWidget *frame299;
  GtkWidget *alignment433;
  GtkWidget *alignment434;
  GtkWidget *hbox335;
  GtkWidget *label11294;
  GtkWidget *scrolledwindow166;
  GtkWidget *textview125;
  GtkWidget *frame300;
  GtkWidget *alignment435;
  GtkWidget *hbox336;
  GtkWidget *entry74;
  GtkWidget *button169;
  GtkWidget *image207;
  GtkWidget *label11295;
  GtkWidget *label11296;
  GtkWidget *frame301;
  GtkWidget *alignment436;
  GtkWidget *alignment437;
  GtkWidget *hbox337;
  GtkWidget *label11297;
  GtkWidget *scrolledwindow167;
  GtkWidget *textview126;
  GtkWidget *label11298;
  GtkWidget *frame302;
  GtkWidget *alignment438;
  GtkWidget *hbox338;
  GtkWidget *label11299;
  GtkWidget *scrolledwindow168;
  GtkWidget *textview127;
  GtkWidget *frame303;
  GtkWidget *alignment439;
  GtkWidget *hbox339;
  GtkWidget *entry75;
  GtkWidget *button170;
  GtkWidget *image208;
  GtkWidget *label11300;
  GtkWidget *label11301;
  GtkWidget *frame304;
  GtkWidget *alignment440;
  GtkWidget *alignment441;
  GtkWidget *hbox340;
  GtkWidget *label11302;
  GtkWidget *scrolledwindow169;
  GtkWidget *textview128;
  GtkWidget *frame305;
  GtkWidget *alignment442;
  GtkWidget *hbox341;
  GtkWidget *entry76;
  GtkWidget *button171;
  GtkWidget *image209;
  GtkWidget *label11303;
  GtkWidget *label11304;
  GtkWidget *frame306;
  GtkWidget *alignment443;
  GtkWidget *drawingarea12;
  GtkWidget *label11305;
  GtkWidget *frame307;
  GtkWidget *alignment444;
  GtkWidget *alignment445;
  GtkWidget *hbox342;
  GtkWidget *label11306;
  GtkWidget *scrolledwindow170;
  GtkWidget *textview129;
  GtkWidget *frame308;
  GtkWidget *alignment446;
  GtkWidget *hbox343;
  GtkWidget *entry77;
  GtkWidget *button172;
  GtkWidget *image210;
  GtkWidget *label11307;
  GtkWidget *label11308;
  GtkWidget *frame309;
  GtkWidget *alignment447;
  GtkWidget *hbox344;
  GtkWidget *frame310;
  GtkWidget *alignment448;
  GtkWidget *scrolledwindow171;
  GtkWidget *textview130;
  GtkWidget *label11309;
  GtkWidget *frame311;
  GtkWidget *alignment449;
  GtkWidget *hbox345;
  GtkWidget *entry78;
  GtkWidget *button173;
  GtkWidget *image211;
  GtkWidget *label11310;
  GtkWidget *label11311;
  GtkWidget *frame312;
  GtkWidget *alignment450;
  GtkWidget *alignment451;
  GtkWidget *hbox346;
  GtkWidget *label11312;
  GtkWidget *scrolledwindow172;
  GtkWidget *textview131;
  GtkWidget *label11313;
  GtkWidget *label666;
  GtkWidget *vbox120;
  GtkWidget *table37;
  GtkWidget *scrolledwindow173;
  GtkWidget *viewport10;
  GtkWidget *vbox121;
  GtkWidget *hbox347;
  GtkWidget *hbox348;
  GtkWidget *label11314;
  GtkWidget *image212;
  GtkWidget *hbox349;
  GtkWidget *label11315;
  GtkWidget *image213;
  GtkWidget *hbox350;
  GtkWidget *label11316;
  GtkWidget *image214;
  GtkWidget *hbox351;
  GtkWidget *label11317;
  GtkWidget *image215;
  GtkWidget *hbox352;
  GtkWidget *label11318;
  GtkWidget *image216;
  GtkWidget *hbox353;
  GtkWidget *label11319;
  GtkWidget *image217;
  GtkWidget *frame313;
  GtkWidget *alignment452;
  GtkWidget *image218;
  GtkWidget *label11320;
  GtkWidget *hbuttonbox8;
  GtkWidget *cei_sup_button_bt1;
  GtkWidget *cei_sup_button_bt2;
  GtkWidget *button176;
  GtkWidget *button177;
  GtkWidget *button178;
  GtkWidget *image219;
  GtkWidget *vbox122;
  GtkWidget *table38;
  GtkWidget *vbox123;
  GtkWidget *frame314;
  GtkWidget *alignment453;
  GtkWidget *hbox354;
  GtkWidget *label11321;
  GtkWidget *scrolledwindow174;
  GtkWidget *textview132;
  GtkWidget *label11322;
  GtkWidget *vbox124;
  GtkWidget *frame315;
  GtkWidget *alignment454;
  GtkWidget *alignment455;
  GtkWidget *hbox355;
  GtkWidget *label11323;
  GtkWidget *scrolledwindow175;
  GtkWidget *textview133;
  GtkWidget *frame316;
  GtkWidget *alignment456;
  GtkWidget *hbox356;
  GtkWidget *entry79;
  GtkWidget *button179;
  GtkWidget *image220;
  GtkWidget *label11324;
  GtkWidget *label11325;
  GtkWidget *frame317;
  GtkWidget *alignment457;
  GtkWidget *alignment458;
  GtkWidget *hbox357;
  GtkWidget *label11326;
  GtkWidget *scrolledwindow176;
  GtkWidget *textview134;
  GtkWidget *label11327;
  GtkWidget *frame318;
  GtkWidget *alignment459;
  GtkWidget *hbox358;
  GtkWidget *label11328;
  GtkWidget *scrolledwindow177;
  GtkWidget *textview135;
  GtkWidget *frame319;
  GtkWidget *alignment460;
  GtkWidget *hbox359;
  GtkWidget *entry80;
  GtkWidget *button180;
  GtkWidget *image221;
  GtkWidget *label11329;
  GtkWidget *label11330;
  GtkWidget *frame320;
  GtkWidget *alignment461;
  GtkWidget *alignment462;
  GtkWidget *hbox360;
  GtkWidget *label11331;
  GtkWidget *scrolledwindow178;
  GtkWidget *textview136;
  GtkWidget *frame321;
  GtkWidget *alignment463;
  GtkWidget *hbox361;
  GtkWidget *entry81;
  GtkWidget *button181;
  GtkWidget *image222;
  GtkWidget *label11332;
  GtkWidget *label11333;
  GtkWidget *frame322;
  GtkWidget *alignment464;
  GtkWidget *drawingarea13;
  GtkWidget *label11334;
  GtkWidget *frame323;
  GtkWidget *alignment465;
  GtkWidget *alignment466;
  GtkWidget *hbox362;
  GtkWidget *label11335;
  GtkWidget *scrolledwindow179;
  GtkWidget *textview137;
  GtkWidget *frame324;
  GtkWidget *alignment467;
  GtkWidget *hbox363;
  GtkWidget *entry82;
  GtkWidget *button182;
  GtkWidget *image223;
  GtkWidget *label11336;
  GtkWidget *label11337;
  GtkWidget *frame325;
  GtkWidget *alignment468;
  GtkWidget *hbox364;
  GtkWidget *frame326;
  GtkWidget *alignment469;
  GtkWidget *scrolledwindow180;
  GtkWidget *textview138;
  GtkWidget *label11338;
  GtkWidget *frame327;
  GtkWidget *alignment470;
  GtkWidget *hbox365;
  GtkWidget *entry83;
  GtkWidget *button183;
  GtkWidget *image224;
  GtkWidget *label11339;
  GtkWidget *label11340;
  GtkWidget *frame328;
  GtkWidget *alignment471;
  GtkWidget *alignment472;
  GtkWidget *hbox366;
  GtkWidget *label11341;
  GtkWidget *scrolledwindow181;
  GtkWidget *textview139;
  GtkWidget *label11342;
  GtkWidget *label20;
  GtkWidget *vbox125;
  GtkWidget *table39;
  GtkWidget *scrolledwindow182;
  GtkWidget *viewport11;
  GtkWidget *vbox126;
  GtkWidget *hbox367;
  GtkWidget *hbox368;
  GtkWidget *label11343;
  GtkWidget *image225;
  GtkWidget *hbox369;
  GtkWidget *label11344;
  GtkWidget *image226;
  GtkWidget *hbox370;
  GtkWidget *label11345;
  GtkWidget *image227;
  GtkWidget *hbox371;
  GtkWidget *label11346;
  GtkWidget *image228;
  GtkWidget *hbox372;
  GtkWidget *label11347;
  GtkWidget *image229;
  GtkWidget *hbox373;
  GtkWidget *label11348;
  GtkWidget *image230;
  GtkWidget *frame329;
  GtkWidget *alignment473;
  GtkWidget *image231;
  GtkWidget *label11349;
  GtkWidget *hbuttonbox9;
  GtkWidget *button184;
  GtkWidget *button185;
  GtkWidget *button186;
  GtkWidget *button187;
  GtkWidget *button188;
  GtkWidget *image232;
  GtkWidget *vbox127;
  GtkWidget *table40;
  GtkWidget *vbox128;
  GtkWidget *frame330;
  GtkWidget *alignment474;
  GtkWidget *hbox374;
  GtkWidget *label11350;
  GtkWidget *scrolledwindow183;
  GtkWidget *textview140;
  GtkWidget *label11351;
  GtkWidget *vbox129;
  GtkWidget *frame331;
  GtkWidget *alignment475;
  GtkWidget *alignment476;
  GtkWidget *hbox375;
  GtkWidget *label11352;
  GtkWidget *scrolledwindow184;
  GtkWidget *textview141;
  GtkWidget *frame332;
  GtkWidget *alignment477;
  GtkWidget *hbox376;
  GtkWidget *entry84;
  GtkWidget *button189;
  GtkWidget *image233;
  GtkWidget *label11353;
  GtkWidget *label11354;
  GtkWidget *frame333;
  GtkWidget *alignment478;
  GtkWidget *alignment479;
  GtkWidget *hbox377;
  GtkWidget *label11355;
  GtkWidget *scrolledwindow185;
  GtkWidget *textview142;
  GtkWidget *label11356;
  GtkWidget *frame334;
  GtkWidget *alignment480;
  GtkWidget *hbox378;
  GtkWidget *label11357;
  GtkWidget *scrolledwindow186;
  GtkWidget *textview143;
  GtkWidget *frame335;
  GtkWidget *alignment481;
  GtkWidget *hbox379;
  GtkWidget *entry85;
  GtkWidget *button190;
  GtkWidget *image234;
  GtkWidget *label11358;
  GtkWidget *label11359;
  GtkWidget *frame336;
  GtkWidget *alignment482;
  GtkWidget *alignment483;
  GtkWidget *hbox380;
  GtkWidget *label11360;
  GtkWidget *scrolledwindow187;
  GtkWidget *textview144;
  GtkWidget *frame337;
  GtkWidget *alignment484;
  GtkWidget *hbox381;
  GtkWidget *entry86;
  GtkWidget *button191;
  GtkWidget *image235;
  GtkWidget *label11361;
  GtkWidget *label11362;
  GtkWidget *frame338;
  GtkWidget *alignment485;
  GtkWidget *drawingarea14;
  GtkWidget *label11363;
  GtkWidget *frame339;
  GtkWidget *alignment486;
  GtkWidget *alignment487;
  GtkWidget *hbox382;
  GtkWidget *label11364;
  GtkWidget *scrolledwindow188;
  GtkWidget *textview145;
  GtkWidget *frame340;
  GtkWidget *alignment488;
  GtkWidget *hbox383;
  GtkWidget *entry87;
  GtkWidget *button192;
  GtkWidget *image236;
  GtkWidget *label11365;
  GtkWidget *label11366;
  GtkWidget *frame341;
  GtkWidget *alignment489;
  GtkWidget *hbox384;
  GtkWidget *frame342;
  GtkWidget *alignment490;
  GtkWidget *scrolledwindow189;
  GtkWidget *textview146;
  GtkWidget *label11367;
  GtkWidget *frame343;
  GtkWidget *alignment491;
  GtkWidget *hbox385;
  GtkWidget *entry88;
  GtkWidget *button193;
  GtkWidget *image237;
  GtkWidget *label11368;
  GtkWidget *label11369;
  GtkWidget *frame344;
  GtkWidget *alignment492;
  GtkWidget *alignment493;
  GtkWidget *hbox386;
  GtkWidget *label11370;
  GtkWidget *scrolledwindow190;
  GtkWidget *textview147;
  GtkWidget *label11371;
  GtkWidget *label53;
  GtkWidget *vbox10;
  GtkWidget *vbox22;
  GtkWidget *toolbar7;
  GtkWidget *toolitem31;
  GtkWidget *uart_cb_cb1;
  GtkWidget *toolitem33;
  GtkWidget *uart_button_clear1;
  GtkWidget *toolitem34;
  GtkWidget *uart_chk_keep1;
  GtkWidget *scrolledwindow9;
  GtkWidget *uart_zt_uart1;
  GtkWidget *vbox23;
  GtkWidget *toolbar8;
  GtkWidget *toolitem38;
  GtkWidget *uart_cb_cb2;
  GtkWidget *toolitem39;
  GtkWidget *uart_button_clear2;
  GtkWidget *toolitem40;
  GtkWidget *uart_chk_keep2;
  GtkWidget *scrolledwindow10;
  GtkWidget *uart_zt_uart2;
  GtkWidget *vbox24;
  GtkWidget *label28;
  GtkWidget *scrolledwindow153;
  GtkWidget *terminal_zt_user;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  Fenetre = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (Fenetre), _("EVE"));
  gtk_window_set_default_size (GTK_WINDOW (Fenetre), 1280, 720);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (Fenetre), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  menuitem10 = gtk_menu_item_new_with_mnemonic (_("_Fichier"));
  gtk_widget_show (menuitem10);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem10);

  menu10 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem10), menu10);

  nouveau1 = gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
  gtk_widget_show (nouveau1);
  gtk_container_add (GTK_CONTAINER (menu10), nouveau1);

  ouvrir1 = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
  gtk_widget_show (ouvrir1);
  gtk_container_add (GTK_CONTAINER (menu10), ouvrir1);

  enregistrer1 = gtk_image_menu_item_new_from_stock ("gtk-save", accel_group);
  gtk_widget_show (enregistrer1);
  gtk_container_add (GTK_CONTAINER (menu10), enregistrer1);

  enregistrer_sous1 = gtk_image_menu_item_new_from_stock ("gtk-save-as", accel_group);
  gtk_widget_show (enregistrer_sous1);
  gtk_container_add (GTK_CONTAINER (menu10), enregistrer_sous1);

  separatormenuitem1 = gtk_separator_menu_item_new ();
  gtk_widget_show (separatormenuitem1);
  gtk_container_add (GTK_CONTAINER (menu10), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  quitter1 = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  gtk_widget_show (quitter1);
  gtk_container_add (GTK_CONTAINER (menu10), quitter1);

  menuitem11 = gtk_menu_item_new_with_mnemonic (_("\303\211_dition"));
  gtk_widget_show (menuitem11);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem11);

  menu11 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem11), menu11);

  couper1 = gtk_image_menu_item_new_from_stock ("gtk-cut", accel_group);
  gtk_widget_show (couper1);
  gtk_container_add (GTK_CONTAINER (menu11), couper1);

  copier1 = gtk_image_menu_item_new_from_stock ("gtk-copy", accel_group);
  gtk_widget_show (copier1);
  gtk_container_add (GTK_CONTAINER (menu11), copier1);

  coller1 = gtk_image_menu_item_new_from_stock ("gtk-paste", accel_group);
  gtk_widget_show (coller1);
  gtk_container_add (GTK_CONTAINER (menu11), coller1);

  supprimer1 = gtk_image_menu_item_new_from_stock ("gtk-delete", accel_group);
  gtk_widget_show (supprimer1);
  gtk_container_add (GTK_CONTAINER (menu11), supprimer1);

  menuitem12 = gtk_menu_item_new_with_mnemonic (_("_Affichage"));
  gtk_widget_show (menuitem12);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem12);

  menu12 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem12), menu12);

  menuitem13 = gtk_menu_item_new_with_mnemonic (_("Aid_e"));
  gtk_widget_show (menuitem13);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem13);

  menu13 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem13), menu13);

  ___propos1 = gtk_menu_item_new_with_mnemonic (_("\303\200 _propos"));
  gtk_widget_show (___propos1);
  gtk_container_add (GTK_CONTAINER (menu13), ___propos1);

  hbox1 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (vbox1), hbox1, TRUE, TRUE, 0);

  vbox7 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox7);
  gtk_box_pack_start (GTK_BOX (hbox1), vbox7, FALSE, FALSE, 0);

  vbox8 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox8);
  gtk_box_pack_start (GTK_BOX (vbox7), vbox8, TRUE, TRUE, 0);

  hbox2 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox2);
  gtk_box_pack_start (GTK_BOX (vbox8), hbox2, FALSE, FALSE, 0);

  label14 = gtk_label_new (_("Can"));
  gtk_widget_show (label14);
  gtk_box_pack_start (GTK_BOX (hbox2), label14, TRUE, FALSE, 0);

  can_button_clear = gtk_button_new_with_mnemonic (_("Clear"));
  gtk_widget_show (can_button_clear);
  gtk_box_pack_end (GTK_BOX (hbox2), can_button_clear, FALSE, FALSE, 0);

  vbox9 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox9);
  gtk_box_pack_start (GTK_BOX (vbox8), vbox9, TRUE, TRUE, 0);

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (vbox9), scrolledwindow2, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_SHADOW_IN);

  can_zt_main = gtk_text_view_new ();
  gtk_widget_show (can_zt_main);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), can_zt_main);
  gtk_widget_set_size_request (can_zt_main, 250, 283);
  gtk_text_buffer_set_text (gtk_text_view_get_buffer (GTK_TEXT_VIEW (can_zt_main)), _("zadazd"), -1);

  notebook2 = gtk_notebook_new ();
  gtk_widget_show (notebook2);
  gtk_box_pack_start (GTK_BOX (vbox7), notebook2, TRUE, TRUE, 0);
  gtk_widget_set_size_request (notebook2, 334, -1);

  scrolledwindow11 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow11);
  gtk_container_add (GTK_CONTAINER (notebook2), scrolledwindow11);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow11), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow11), GTK_SHADOW_IN);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow11), viewport1);

  vbox27 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox27);
  gtk_container_add (GTK_CONTAINER (viewport1), vbox27);

  table5 = gtk_table_new (1, 2, TRUE);
  gtk_widget_show (table5);
  gtk_box_pack_start (GTK_BOX (vbox27), table5, FALSE, TRUE, 0);
  gtk_widget_set_size_request (table5, 255, -1);
  gtk_table_set_col_spacings (GTK_TABLE (table5), 15);

  label34 = gtk_label_new (_("Description"));
  gtk_widget_show (label34);
  gtk_table_attach (GTK_TABLE (table5), label34, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label34), 0, 0.5);
  gtk_misc_set_padding (GTK_MISC (label34), 32, 0);

  label35 = gtk_label_new (_("Message"));
  gtk_widget_show (label35);
  gtk_table_attach (GTK_TABLE (table5), label35, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_size_request (label35, 162, -1);
  gtk_misc_set_alignment (GTK_MISC (label35), 0.66, 0.5);
  gtk_misc_set_padding (GTK_MISC (label35), 60, 0);

  scrolledwindow12 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow12);
  gtk_box_pack_start (GTK_BOX (vbox27), scrolledwindow12, TRUE, TRUE, 0);

  viewport2 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow12), viewport2);

  table6 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table6);
  gtk_container_add (GTK_CONTAINER (viewport2), table6);

  can_msg_button_bt1 = gtk_button_new_with_mnemonic (_("->"));
  gtk_widget_show (can_msg_button_bt1);
  gtk_table_attach (GTK_TABLE (table6), can_msg_button_bt1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  button32 = gtk_button_new_with_mnemonic (_("->"));
  gtk_widget_show (button32);
  gtk_table_attach (GTK_TABLE (table6), button32, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  button33 = gtk_button_new_with_mnemonic (_("->"));
  gtk_widget_show (button33);
  gtk_table_attach (GTK_TABLE (table6), button33, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  scrolledwindow13 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow13);
  gtk_table_attach (GTK_TABLE (table6), scrolledwindow13, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow13), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow13), GTK_SHADOW_IN);

  can_msg_zt_desc1 = gtk_text_view_new ();
  gtk_widget_show (can_msg_zt_desc1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow13), can_msg_zt_desc1);

  scrolledwindow14 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow14);
  gtk_table_attach (GTK_TABLE (table6), scrolledwindow14, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (scrolledwindow14, 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow14), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow14), GTK_SHADOW_IN);

  can_msg_zt_msg1 = gtk_text_view_new ();
  gtk_widget_show (can_msg_zt_msg1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow14), can_msg_zt_msg1);

  scrolledwindow15 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow15);
  gtk_table_attach (GTK_TABLE (table6), scrolledwindow15, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (scrolledwindow15, 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow15), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow15), GTK_SHADOW_IN);

  textview11 = gtk_text_view_new ();
  gtk_widget_show (textview11);
  gtk_container_add (GTK_CONTAINER (scrolledwindow15), textview11);

  scrolledwindow16 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow16);
  gtk_table_attach (GTK_TABLE (table6), scrolledwindow16, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (scrolledwindow16, 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow16), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow16), GTK_SHADOW_IN);

  textview12 = gtk_text_view_new ();
  gtk_widget_show (textview12);
  gtk_container_add (GTK_CONTAINER (scrolledwindow16), textview12);

  scrolledwindow17 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow17);
  gtk_table_attach (GTK_TABLE (table6), scrolledwindow17, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (scrolledwindow17, 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow17), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow17), GTK_SHADOW_IN);

  textview13 = gtk_text_view_new ();
  gtk_widget_show (textview13);
  gtk_container_add (GTK_CONTAINER (scrolledwindow17), textview13);

  scrolledwindow18 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow18);
  gtk_table_attach (GTK_TABLE (table6), scrolledwindow18, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow18), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow18), GTK_SHADOW_IN);

  textview14 = gtk_text_view_new ();
  gtk_widget_show (textview14);
  gtk_container_add (GTK_CONTAINER (scrolledwindow18), textview14);

  label15 = gtk_label_new (_("Messages"));
  gtk_widget_show (label15);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 0), label15);
  gtk_misc_set_alignment (GTK_MISC (label15), 0.91, 0.5);

  vbox28 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox28);
  gtk_container_add (GTK_CONTAINER (notebook2), vbox28);
  gtk_widget_set_size_request (vbox28, 0, -1);

  can_send_st_main = gtk_entry_new ();
  gtk_widget_show (can_send_st_main);
  gtk_box_pack_start (GTK_BOX (vbox28), can_send_st_main, FALSE, FALSE, 65);
  gtk_entry_set_max_length (GTK_ENTRY (can_send_st_main), 26);
  gtk_entry_set_invisible_char (GTK_ENTRY (can_send_st_main), 9679);

  hbox9 = gtk_hbox_new (TRUE, 0);
  gtk_widget_show (hbox9);
  gtk_box_pack_start (GTK_BOX (vbox28), hbox9, TRUE, TRUE, 0);
  gtk_widget_set_size_request (hbox9, -1, 1);

  can_send_button_add = gtk_button_new_with_mnemonic (_("Ajouter "));
  gtk_widget_show (can_send_button_add);
  gtk_box_pack_end (GTK_BOX (hbox9), can_send_button_add, FALSE, TRUE, 0);

  can_send_button_send = gtk_button_new_with_mnemonic (_("Envoyer"));
  gtk_widget_show (can_send_button_send);
  gtk_box_pack_start (GTK_BOX (hbox9), can_send_button_send, FALSE, TRUE, 0);

  label10 = gtk_label_new (_("Envoi CAN"));
  gtk_widget_show (label10);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 1), label10);
  gtk_misc_set_alignment (GTK_MISC (label10), 0.49, 0.5);

  vbox41 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox41);
  gtk_container_add (GTK_CONTAINER (notebook2), vbox41);

  frame13 = gtk_frame_new (NULL);
  gtk_widget_show (frame13);
  gtk_box_pack_start (GTK_BOX (vbox41), frame13, TRUE, TRUE, 0);
  gtk_widget_set_sensitive (frame13, FALSE);

  alignment28 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment28);
  gtk_container_add (GTK_CONTAINER (frame13), alignment28);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment28), 0, 0, 12, 0);

  table41 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table41);
  gtk_container_add (GTK_CONTAINER (alignment28), table41);

  can_filt_chk_all = gtk_check_button_new_with_mnemonic (_("Tous"));
  gtk_widget_show (can_filt_chk_all);
  gtk_table_attach (GTK_TABLE (table41), can_filt_chk_all, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  can_filt_chk_bal = gtk_check_button_new_with_mnemonic (_("Balises"));
  gtk_widget_show (can_filt_chk_bal);
  gtk_table_attach (GTK_TABLE (table41), can_filt_chk_bal, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  can_filt_chk_sup = gtk_check_button_new_with_mnemonic (_("Supervision"));
  gtk_widget_show (can_filt_chk_sup);
  gtk_table_attach (GTK_TABLE (table41), can_filt_chk_sup, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  can_filt_chk_stra = gtk_check_button_new_with_mnemonic (_("Strat\303\251gie"));
  gtk_widget_show (can_filt_chk_stra);
  gtk_table_attach (GTK_TABLE (table41), can_filt_chk_stra, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  can_filt_chk_pro = gtk_check_button_new_with_mnemonic (_("Propulsion"));
  gtk_widget_show (can_filt_chk_pro);
  gtk_table_attach (GTK_TABLE (table41), can_filt_chk_pro, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  can_filt_chk_act = gtk_check_button_new_with_mnemonic (_("Actionneurs"));
  gtk_widget_show (can_filt_chk_act);
  gtk_table_attach (GTK_TABLE (table41), can_filt_chk_act, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label48 = gtk_label_new (_("Afficher carte"));
  gtk_widget_show (label48);
  gtk_frame_set_label_widget (GTK_FRAME (frame13), label48);
  gtk_label_set_use_markup (GTK_LABEL (label48), TRUE);

  frame14 = gtk_frame_new (NULL);
  gtk_widget_show (frame14);
  gtk_box_pack_start (GTK_BOX (vbox41), frame14, TRUE, TRUE, 0);

  alignment29 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment29);
  gtk_container_add (GTK_CONTAINER (frame14), alignment29);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment29), 0, 0, 12, 0);

  hbox19 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox19);
  gtk_container_add (GTK_CONTAINER (alignment29), hbox19);

  frame15 = gtk_frame_new (NULL);
  gtk_widget_show (frame15);
  gtk_box_pack_start (GTK_BOX (hbox19), frame15, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame15), GTK_SHADOW_NONE);

  alignment30 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment30);
  gtk_container_add (GTK_CONTAINER (frame15), alignment30);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment30), 0, 0, 12, 0);

  can_filt_st_sid = gtk_entry_new ();
  gtk_widget_show (can_filt_st_sid);
  gtk_container_add (GTK_CONTAINER (alignment30), can_filt_st_sid);
  gtk_widget_set_size_request (can_filt_st_sid, 4, -1);
  gtk_entry_set_max_length (GTK_ENTRY (can_filt_st_sid), 4);
  gtk_entry_set_invisible_char (GTK_ENTRY (can_filt_st_sid), 8226);
  gtk_entry_set_width_chars (GTK_ENTRY (can_filt_st_sid), 2);

  label50 = gtk_label_new (_("SID"));
  gtk_widget_show (label50);
  gtk_frame_set_label_widget (GTK_FRAME (frame15), label50);
  gtk_label_set_use_markup (GTK_LABEL (label50), TRUE);

  frame16 = gtk_frame_new (NULL);
  gtk_widget_show (frame16);
  gtk_box_pack_start (GTK_BOX (hbox19), frame16, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame16, 109, -1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame16), GTK_SHADOW_NONE);

  alignment31 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment31);
  gtk_container_add (GTK_CONTAINER (frame16), alignment31);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment31), 0, 0, 12, 0);

  can_filt_st_data = gtk_entry_new ();
  gtk_widget_show (can_filt_st_data);
  gtk_container_add (GTK_CONTAINER (alignment31), can_filt_st_data);
  gtk_widget_set_size_request (can_filt_st_data, 221, -1);
  gtk_entry_set_max_length (GTK_ENTRY (can_filt_st_data), 16);
  gtk_entry_set_invisible_char (GTK_ENTRY (can_filt_st_data), 8226);
  gtk_entry_set_width_chars (GTK_ENTRY (can_filt_st_data), 16);

  label51 = gtk_label_new (_("Data"));
  gtk_widget_show (label51);
  gtk_frame_set_label_widget (GTK_FRAME (frame16), label51);
  gtk_label_set_use_markup (GTK_LABEL (label51), TRUE);

  frame17 = gtk_frame_new (NULL);
  gtk_widget_show (frame17);
  gtk_box_pack_start (GTK_BOX (hbox19), frame17, TRUE, TRUE, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame17), GTK_SHADOW_NONE);

  alignment32 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment32);
  gtk_container_add (GTK_CONTAINER (frame17), alignment32);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment32), 0, 0, 12, 0);

  can_filt_st_taille = gtk_entry_new ();
  gtk_widget_show (can_filt_st_taille);
  gtk_container_add (GTK_CONTAINER (alignment32), can_filt_st_taille);
  gtk_widget_set_size_request (can_filt_st_taille, 20, -1);
  gtk_entry_set_max_length (GTK_ENTRY (can_filt_st_taille), 2);
  gtk_entry_set_invisible_char (GTK_ENTRY (can_filt_st_taille), 8226);

  label52 = gtk_label_new (_("Taille"));
  gtk_widget_show (label52);
  gtk_frame_set_label_widget (GTK_FRAME (frame17), label52);
  gtk_label_set_use_markup (GTK_LABEL (label52), TRUE);

  can_filt_button_ok = gtk_button_new ();
  gtk_widget_show (can_filt_button_ok);
  gtk_box_pack_start (GTK_BOX (hbox19), can_filt_button_ok, FALSE, FALSE, 0);

  image20 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image20);
  gtk_container_add (GTK_CONTAINER (can_filt_button_ok), image20);

  label49 = gtk_label_new (_("Filtres"));
  gtk_widget_show (label49);
  gtk_frame_set_label_widget (GTK_FRAME (frame14), label49);
  gtk_label_set_use_markup (GTK_LABEL (label49), TRUE);

  label11 = gtk_label_new (_("Filtres"));
  gtk_widget_show (label11);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook2), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook2), 2), label11);

  hbox3 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox3);
  gtk_box_pack_start (GTK_BOX (hbox1), hbox3, TRUE, TRUE, 0);

  vbox11 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox11);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox11, TRUE, TRUE, 0);

  toolbar20 = gtk_toolbar_new ();
  gtk_widget_show (toolbar20);
  gtk_box_pack_start (GTK_BOX (vbox11), toolbar20, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar20), GTK_TOOLBAR_BOTH_HORIZ);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar20));

  toolitem104 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem104);
  gtk_container_add (GTK_CONTAINER (toolbar20), toolitem104);

  os_button_play = gtk_button_new ();
  gtk_widget_show (os_button_play);
  gtk_container_add (GTK_CONTAINER (toolitem104), os_button_play);

  alignment300 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment300);
  gtk_container_add (GTK_CONTAINER (os_button_play), alignment300);

  hbox191 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox191);
  gtk_container_add (GTK_CONTAINER (alignment300), hbox191);

  image108 = gtk_image_new_from_stock ("gtk-media-play", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image108);
  gtk_box_pack_start (GTK_BOX (hbox191), image108, FALSE, FALSE, 0);

  label339 = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label339);
  gtk_box_pack_start (GTK_BOX (hbox191), label339, FALSE, FALSE, 0);

  toolitem105 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem105);
  gtk_container_add (GTK_CONTAINER (toolbar20), toolitem105);

  os_button_pause = gtk_button_new ();
  gtk_widget_show (os_button_pause);
  gtk_container_add (GTK_CONTAINER (toolitem105), os_button_pause);
  gtk_widget_set_sensitive (os_button_pause, FALSE);

  alignment301 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment301);
  gtk_container_add (GTK_CONTAINER (os_button_pause), alignment301);

  hbox192 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox192);
  gtk_container_add (GTK_CONTAINER (alignment301), hbox192);

  image109 = gtk_image_new_from_stock ("gtk-media-pause", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image109);
  gtk_box_pack_start (GTK_BOX (hbox192), image109, FALSE, FALSE, 0);

  label340 = gtk_label_new_with_mnemonic ("");
  gtk_widget_show (label340);
  gtk_box_pack_start (GTK_BOX (hbox192), label340, FALSE, FALSE, 0);

  toolitem106 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem106);
  gtk_container_add (GTK_CONTAINER (toolbar20), toolitem106);

  os_button_stop = gtk_button_new ();
  gtk_widget_show (os_button_stop);
  gtk_container_add (GTK_CONTAINER (toolitem106), os_button_stop);
  gtk_widget_set_sensitive (os_button_stop, FALSE);

  image110 = gtk_image_new_from_stock ("gtk-media-stop", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image110);
  gtk_container_add (GTK_CONTAINER (os_button_stop), image110);

  toolitem107 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem107);
  gtk_container_add (GTK_CONTAINER (toolbar20), toolitem107);

  frame199 = gtk_frame_new (NULL);
  gtk_widget_show (frame199);
  gtk_container_add (GTK_CONTAINER (toolitem107), frame199);
  gtk_frame_set_shadow_type (GTK_FRAME (frame199), GTK_SHADOW_NONE);

  alignment302 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment302);
  gtk_container_add (GTK_CONTAINER (frame199), alignment302);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment302), 0, 0, 12, 0);

  hbox193 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox193);
  gtk_container_add (GTK_CONTAINER (alignment302), hbox193);

  os_st_timeJump = gtk_entry_new ();
  gtk_widget_show (os_st_timeJump);
  gtk_box_pack_start (GTK_BOX (hbox193), os_st_timeJump, TRUE, TRUE, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (os_st_timeJump), 8226);

  os_button_jumpOk = gtk_button_new_with_mnemonic (_("ok"));
  gtk_widget_show (os_button_jumpOk);
  gtk_box_pack_start (GTK_BOX (hbox193), os_button_jumpOk, FALSE, FALSE, 0);

  label341 = gtk_label_new (_("Saut au temps"));
  gtk_widget_show (label341);
  gtk_frame_set_label_widget (GTK_FRAME (frame199), label341);
  gtk_label_set_use_markup (GTK_LABEL (label341), TRUE);

  toolitem108 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem108);
  gtk_container_add (GTK_CONTAINER (toolbar20), toolitem108);

  frame200 = gtk_frame_new (NULL);
  gtk_widget_show (frame200);
  gtk_container_add (GTK_CONTAINER (toolitem108), frame200);
  gtk_frame_set_shadow_type (GTK_FRAME (frame200), GTK_SHADOW_NONE);

  alignment303 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment303);
  gtk_container_add (GTK_CONTAINER (frame200), alignment303);
  gtk_widget_set_size_request (alignment303, 2, 6);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment303), 0, 0, 12, 0);

  scrolledwindow114 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow114);
  gtk_container_add (GTK_CONTAINER (alignment303), scrolledwindow114);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow114), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow114), GTK_SHADOW_IN);

  os_zt_CurrentTime = gtk_text_view_new ();
  gtk_widget_show (os_zt_CurrentTime);
  gtk_container_add (GTK_CONTAINER (scrolledwindow114), os_zt_CurrentTime);
  gtk_text_view_set_editable (GTK_TEXT_VIEW (os_zt_CurrentTime), FALSE);
  gtk_text_view_set_pixels_above_lines (GTK_TEXT_VIEW (os_zt_CurrentTime), 3);

  label342 = gtk_label_new (_("<b>Temps actuel</b>"));
  gtk_widget_show (label342);
  gtk_frame_set_label_widget (GTK_FRAME (frame200), label342);
  gtk_label_set_use_markup (GTK_LABEL (label342), TRUE);

  toolitem109 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem109);
  gtk_container_add (GTK_CONTAINER (toolbar20), toolitem109);

  frame202 = gtk_frame_new (NULL);
  gtk_widget_show (frame202);
  gtk_container_add (GTK_CONTAINER (toolitem109), frame202);
  gtk_frame_set_shadow_type (GTK_FRAME (frame202), GTK_SHADOW_NONE);

  alignment305 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment305);
  gtk_container_add (GTK_CONTAINER (frame202), alignment305);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment305), 0, 0, 12, 0);

  scrolledwindow116 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow116);
  gtk_container_add (GTK_CONTAINER (alignment305), scrolledwindow116);
  gtk_widget_set_size_request (scrolledwindow116, -1, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow116), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow116), GTK_SHADOW_IN);

  os_zt_matchTime = gtk_text_view_new ();
  gtk_widget_show (os_zt_matchTime);
  gtk_container_add (GTK_CONTAINER (scrolledwindow116), os_zt_matchTime);

  label350 = gtk_label_new (_("<b>Temps de match</b>"));
  gtk_widget_show (label350);
  gtk_frame_set_label_widget (GTK_FRAME (frame202), label350);
  gtk_label_set_use_markup (GTK_LABEL (label350), TRUE);

  cei_ong_all = gtk_notebook_new ();
  gtk_widget_show (cei_ong_all);
  gtk_box_pack_start (GTK_BOX (vbox11), cei_ong_all, TRUE, TRUE, 0);
  gtk_widget_set_sensitive (cei_ong_all, FALSE);

  vbox102 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox102);
  gtk_container_add (GTK_CONTAINER (cei_ong_all), vbox102);

  table30 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table30);
  gtk_box_pack_start (GTK_BOX (vbox102), table30, TRUE, TRUE, 0);

  scrolledwindow144 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow144);
  gtk_table_attach (GTK_TABLE (table30), scrolledwindow144, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow144), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow144), GTK_SHADOW_IN);

  viewport7 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport7);
  gtk_container_add (GTK_CONTAINER (scrolledwindow144), viewport7);
  gtk_widget_set_size_request (viewport7, 42, 196);

  vbox103 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox103);
  gtk_container_add (GTK_CONTAINER (viewport7), vbox103);

  hbox269 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox269);
  gtk_box_pack_start (GTK_BOX (vbox103), hbox269, TRUE, TRUE, 0);

  hbox270 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox270);
  gtk_box_pack_start (GTK_BOX (hbox269), hbox270, TRUE, TRUE, 0);

  label11199 = gtk_label_new (_("Error"));
  gtk_widget_show (label11199);
  gtk_box_pack_start (GTK_BOX (hbox270), label11199, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11199), GTK_JUSTIFY_CENTER);

  image165 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image165);
  gtk_box_pack_end (GTK_BOX (hbox270), image165, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image165, FALSE);

  hbox271 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox271);
  gtk_box_pack_start (GTK_BOX (vbox103), hbox271, TRUE, TRUE, 0);

  label11200 = gtk_label_new (_("User"));
  gtk_widget_show (label11200);
  gtk_box_pack_start (GTK_BOX (hbox271), label11200, TRUE, FALSE, 0);

  image166 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image166);
  gtk_box_pack_end (GTK_BOX (hbox271), image166, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image166, FALSE);

  hbox272 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox272);
  gtk_box_pack_start (GTK_BOX (vbox103), hbox272, FALSE, FALSE, 0);

  label11201 = gtk_label_new (_("User"));
  gtk_widget_show (label11201);
  gtk_box_pack_start (GTK_BOX (hbox272), label11201, TRUE, FALSE, 0);

  image167 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image167);
  gtk_box_pack_end (GTK_BOX (hbox272), image167, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image167, FALSE);

  hbox273 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox273);
  gtk_box_pack_start (GTK_BOX (vbox103), hbox273, TRUE, TRUE, 0);

  label11202 = gtk_label_new (_("Uart"));
  gtk_widget_show (label11202);
  gtk_box_pack_start (GTK_BOX (hbox273), label11202, TRUE, FALSE, 0);

  image168 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image168);
  gtk_box_pack_end (GTK_BOX (hbox273), image168, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image168, FALSE);

  hbox274 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox274);
  gtk_box_pack_start (GTK_BOX (vbox103), hbox274, TRUE, TRUE, 0);

  label11203 = gtk_label_new (_("Can"));
  gtk_widget_show (label11203);
  gtk_box_pack_start (GTK_BOX (hbox274), label11203, TRUE, FALSE, 0);

  image169 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image169);
  gtk_box_pack_end (GTK_BOX (hbox274), image169, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image169, FALSE);

  hbox275 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox275);
  gtk_box_pack_start (GTK_BOX (vbox103), hbox275, TRUE, TRUE, 0);

  label11204 = gtk_label_new (_("Run"));
  gtk_widget_show (label11204);
  gtk_box_pack_start (GTK_BOX (hbox275), label11204, TRUE, FALSE, 0);

  image170 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image170);
  gtk_box_pack_end (GTK_BOX (hbox275), image170, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image170, FALSE);

  frame251 = gtk_frame_new (NULL);
  gtk_widget_show (frame251);
  gtk_table_attach (GTK_TABLE (table30), frame251, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame251, 54, 43);

  alignment369 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment369);
  gtk_container_add (GTK_CONTAINER (frame251), alignment369);
  gtk_widget_set_size_request (alignment369, -1, 162);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment369), 0, 0, 12, 0);

  image171 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image171);
  gtk_container_add (GTK_CONTAINER (alignment369), image171);
  gtk_widget_set_size_request (image171, -1, 330);
  gtk_widget_set_sensitive (image171, FALSE);

  label11205 = gtk_label_new (_("Carte en\n fonctionnement"));
  gtk_widget_show (label11205);
  gtk_frame_set_label_widget (GTK_FRAME (frame251), label11205);
  gtk_label_set_use_markup (GTK_LABEL (label11205), TRUE);
  gtk_label_set_justify (GTK_LABEL (label11205), GTK_JUSTIFY_CENTER);

  hbuttonbox5 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox5);
  gtk_table_attach (GTK_TABLE (table30), hbuttonbox5, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hbuttonbox5, 449, 2);

  cei_act_button_bt1 = gtk_button_new_with_mnemonic (_("BT1"));
  gtk_widget_show (cei_act_button_bt1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox5), cei_act_button_bt1);
  gtk_container_set_border_width (GTK_CONTAINER (cei_act_button_bt1), 11);
  GTK_WIDGET_SET_FLAGS (cei_act_button_bt1, GTK_CAN_DEFAULT);

  cei_act_button_bt2 = gtk_button_new_with_mnemonic (_("BT2"));
  gtk_widget_show (cei_act_button_bt2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox5), cei_act_button_bt2);
  gtk_container_set_border_width (GTK_CONTAINER (cei_act_button_bt2), 11);
  GTK_WIDGET_SET_FLAGS (cei_act_button_bt2, GTK_CAN_DEFAULT);

  cei_act_button_bt3 = gtk_button_new_with_mnemonic (_("BT3"));
  gtk_widget_show (cei_act_button_bt3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox5), cei_act_button_bt3);
  gtk_container_set_border_width (GTK_CONTAINER (cei_act_button_bt3), 11);
  GTK_WIDGET_SET_FLAGS (cei_act_button_bt3, GTK_CAN_DEFAULT);

  cei_act_button_bt4 = gtk_button_new_with_mnemonic (_("BT4"));
  gtk_widget_show (cei_act_button_bt4);
  gtk_container_add (GTK_CONTAINER (hbuttonbox5), cei_act_button_bt4);
  gtk_container_set_border_width (GTK_CONTAINER (cei_act_button_bt4), 11);
  GTK_WIDGET_SET_FLAGS (cei_act_button_bt4, GTK_CAN_DEFAULT);

  cei_act_button_reset = gtk_button_new_with_mnemonic (_("Reset"));
  gtk_widget_show (cei_act_button_reset);
  gtk_container_add (GTK_CONTAINER (hbuttonbox5), cei_act_button_reset);
  gtk_container_set_border_width (GTK_CONTAINER (cei_act_button_reset), 11);
  GTK_WIDGET_SET_FLAGS (cei_act_button_reset, GTK_CAN_DEFAULT);

  image172 = create_pixmap (Fenetre, "carte.png");
  gtk_widget_show (image172);
  gtk_table_attach (GTK_TABLE (table30), image172, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);

  vbox104 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox104);
  gtk_box_pack_start (GTK_BOX (vbox102), vbox104, TRUE, TRUE, 0);

  table31 = gtk_table_new (3, 3, TRUE);
  gtk_widget_show (table31);
  gtk_box_pack_start (GTK_BOX (vbox104), table31, TRUE, TRUE, 0);
  gtk_widget_set_size_request (table31, 477, 249);

  vbox105 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox105);
  gtk_table_attach (GTK_TABLE (table31), vbox105, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame252 = gtk_frame_new (NULL);
  gtk_widget_show (frame252);
  gtk_box_pack_start (GTK_BOX (vbox105), frame252, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame252, 41, 15);

  alignment370 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment370);
  gtk_container_add (GTK_CONTAINER (frame252), alignment370);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment370), 0, 0, 12, 0);

  hbox276 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox276);
  gtk_container_add (GTK_CONTAINER (alignment370), hbox276);

  label11206 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11206);
  gtk_box_pack_start (GTK_BOX (hbox276), label11206, FALSE, FALSE, 0);

  scrolledwindow145 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow145);
  gtk_box_pack_start (GTK_BOX (hbox276), scrolledwindow145, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow145), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow145), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow145), GTK_SHADOW_IN);

  textview107 = gtk_text_view_new ();
  gtk_widget_show (textview107);
  gtk_container_add (GTK_CONTAINER (scrolledwindow145), textview107);
  gtk_widget_set_size_request (textview107, -1, 8);

  label11207 = gtk_label_new (_("Step Motor"));
  gtk_widget_show (label11207);
  gtk_frame_set_label_widget (GTK_FRAME (frame252), label11207);
  gtk_label_set_use_markup (GTK_LABEL (label11207), TRUE);

  vbox106 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox106);
  gtk_table_attach (GTK_TABLE (table31), vbox106, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame253 = gtk_frame_new (NULL);
  gtk_widget_show (frame253);
  gtk_box_pack_start (GTK_BOX (vbox106), frame253, TRUE, TRUE, 0);

  alignment371 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment371);
  gtk_container_add (GTK_CONTAINER (frame253), alignment371);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment371), 0, 0, 12, 0);

  alignment372 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment372);
  gtk_container_add (GTK_CONTAINER (alignment371), alignment372);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment372), 8, 8, 5, 0);

  hbox277 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox277);
  gtk_container_add (GTK_CONTAINER (alignment372), hbox277);

  label11208 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11208);
  gtk_box_pack_start (GTK_BOX (hbox277), label11208, FALSE, FALSE, 0);

  scrolledwindow146 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow146);
  gtk_box_pack_start (GTK_BOX (hbox277), scrolledwindow146, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow146), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow146), GTK_SHADOW_IN);

  textview108 = gtk_text_view_new ();
  gtk_widget_show (textview108);
  gtk_container_add (GTK_CONTAINER (scrolledwindow146), textview108);

  frame254 = gtk_frame_new (NULL);
  gtk_widget_show (frame254);
  gtk_box_pack_start (GTK_BOX (hbox277), frame254, FALSE, FALSE, 0);

  alignment373 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment373);
  gtk_container_add (GTK_CONTAINER (frame254), alignment373);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment373), 0, 0, 12, 0);

  hbox278 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox278);
  gtk_container_add (GTK_CONTAINER (alignment373), hbox278);

  entry64 = gtk_entry_new ();
  gtk_widget_show (entry64);
  gtk_box_pack_start (GTK_BOX (hbox278), entry64, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry64, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry64), 8226);

  button144 = gtk_button_new ();
  gtk_widget_show (button144);
  gtk_box_pack_start (GTK_BOX (hbox278), button144, FALSE, FALSE, 0);

  image173 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image173);
  gtk_container_add (GTK_CONTAINER (button144), image173);

  label11209 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11209);
  gtk_frame_set_label_widget (GTK_FRAME (frame254), label11209);
  gtk_label_set_use_markup (GTK_LABEL (label11209), TRUE);

  label11210 = gtk_label_new (_("Extern It"));
  gtk_widget_show (label11210);
  gtk_frame_set_label_widget (GTK_FRAME (frame253), label11210);
  gtk_label_set_use_markup (GTK_LABEL (label11210), TRUE);

  frame255 = gtk_frame_new (NULL);
  gtk_widget_show (frame255);
  gtk_table_attach (GTK_TABLE (table31), frame255, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame255, 83, 9);

  alignment374 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment374);
  gtk_container_add (GTK_CONTAINER (frame255), alignment374);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment374), 0, 0, 12, 0);

  alignment375 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment375);
  gtk_container_add (GTK_CONTAINER (alignment374), alignment375);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment375), 0, 0, 12, 0);

  hbox279 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox279);
  gtk_container_add (GTK_CONTAINER (alignment375), hbox279);

  label11211 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11211);
  gtk_box_pack_start (GTK_BOX (hbox279), label11211, FALSE, FALSE, 0);

  scrolledwindow147 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow147);
  gtk_box_pack_start (GTK_BOX (hbox279), scrolledwindow147, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow147), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow147), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow147), GTK_SHADOW_IN);

  textview109 = gtk_text_view_new ();
  gtk_widget_show (textview109);
  gtk_container_add (GTK_CONTAINER (scrolledwindow147), textview109);
  gtk_widget_set_size_request (textview109, -1, 8);

  label11212 = gtk_label_new (_("Servo"));
  gtk_widget_show (label11212);
  gtk_frame_set_label_widget (GTK_FRAME (frame255), label11212);
  gtk_label_set_use_markup (GTK_LABEL (label11212), TRUE);

  frame256 = gtk_frame_new (NULL);
  gtk_widget_show (frame256);
  gtk_table_attach (GTK_TABLE (table31), frame256, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment376 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment376);
  gtk_container_add (GTK_CONTAINER (frame256), alignment376);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment376), 8, 8, 5, 0);

  hbox280 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox280);
  gtk_container_add (GTK_CONTAINER (alignment376), hbox280);

  label11213 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11213);
  gtk_box_pack_start (GTK_BOX (hbox280), label11213, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label11213, 46, -1);

  scrolledwindow148 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow148);
  gtk_box_pack_start (GTK_BOX (hbox280), scrolledwindow148, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow148), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow148), GTK_SHADOW_IN);

  textview110 = gtk_text_view_new ();
  gtk_widget_show (textview110);
  gtk_container_add (GTK_CONTAINER (scrolledwindow148), textview110);

  frame257 = gtk_frame_new (NULL);
  gtk_widget_show (frame257);
  gtk_box_pack_start (GTK_BOX (hbox280), frame257, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame257), 0.01, 0.5);

  alignment377 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment377);
  gtk_container_add (GTK_CONTAINER (frame257), alignment377);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment377), 0, 0, 12, 0);

  hbox281 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox281);
  gtk_container_add (GTK_CONTAINER (alignment377), hbox281);

  entry65 = gtk_entry_new ();
  gtk_widget_show (entry65);
  gtk_box_pack_start (GTK_BOX (hbox281), entry65, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry65, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry65), 8226);

  button145 = gtk_button_new ();
  gtk_widget_show (button145);
  gtk_box_pack_start (GTK_BOX (hbox281), button145, FALSE, FALSE, 0);

  image174 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image174);
  gtk_container_add (GTK_CONTAINER (button145), image174);

  label11214 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11214);
  gtk_frame_set_label_widget (GTK_FRAME (frame257), label11214);
  gtk_widget_set_size_request (label11214, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11214), TRUE);

  label11215 = gtk_label_new (_("QEI"));
  gtk_widget_show (label11215);
  gtk_frame_set_label_widget (GTK_FRAME (frame256), label11215);
  gtk_label_set_use_markup (GTK_LABEL (label11215), TRUE);

  frame258 = gtk_frame_new (NULL);
  gtk_widget_show (frame258);
  gtk_table_attach (GTK_TABLE (table31), frame258, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment378 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment378);
  gtk_container_add (GTK_CONTAINER (frame258), alignment378);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment378), 0, 0, 12, 0);

  alignment379 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment379);
  gtk_container_add (GTK_CONTAINER (alignment378), alignment379);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment379), 8, 8, 5, 0);

  hbox282 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox282);
  gtk_container_add (GTK_CONTAINER (alignment379), hbox282);

  label11216 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11216);
  gtk_box_pack_start (GTK_BOX (hbox282), label11216, FALSE, FALSE, 0);

  scrolledwindow149 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow149);
  gtk_box_pack_start (GTK_BOX (hbox282), scrolledwindow149, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow149), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow149), GTK_SHADOW_IN);

  textview111 = gtk_text_view_new ();
  gtk_widget_show (textview111);
  gtk_container_add (GTK_CONTAINER (scrolledwindow149), textview111);

  frame259 = gtk_frame_new (NULL);
  gtk_widget_show (frame259);
  gtk_box_pack_start (GTK_BOX (hbox282), frame259, FALSE, FALSE, 0);

  alignment380 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment380);
  gtk_container_add (GTK_CONTAINER (frame259), alignment380);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment380), 0, 0, 12, 0);

  hbox283 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox283);
  gtk_container_add (GTK_CONTAINER (alignment380), hbox283);

  entry66 = gtk_entry_new ();
  gtk_widget_show (entry66);
  gtk_box_pack_start (GTK_BOX (hbox283), entry66, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry66, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry66), 8226);

  button146 = gtk_button_new ();
  gtk_widget_show (button146);
  gtk_box_pack_start (GTK_BOX (hbox283), button146, FALSE, FALSE, 0);

  image175 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image175);
  gtk_container_add (GTK_CONTAINER (button146), image175);

  label11217 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11217);
  gtk_frame_set_label_widget (GTK_FRAME (frame259), label11217);
  gtk_label_set_use_markup (GTK_LABEL (label11217), TRUE);

  label11218 = gtk_label_new (_("DCM"));
  gtk_widget_show (label11218);
  gtk_frame_set_label_widget (GTK_FRAME (frame258), label11218);
  gtk_label_set_use_markup (GTK_LABEL (label11218), TRUE);

  frame260 = gtk_frame_new (NULL);
  gtk_widget_show (frame260);
  gtk_table_attach (GTK_TABLE (table31), frame260, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame260), GTK_SHADOW_NONE);

  alignment381 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment381);
  gtk_container_add (GTK_CONTAINER (frame260), alignment381);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment381), 0, 0, 12, 0);

  drawingarea10 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea10);
  gtk_container_add (GTK_CONTAINER (alignment381), drawingarea10);

  label11219 = gtk_label_new ("");
  gtk_widget_show (label11219);
  gtk_frame_set_label_widget (GTK_FRAME (frame260), label11219);
  gtk_label_set_use_markup (GTK_LABEL (label11219), TRUE);

  frame261 = gtk_frame_new (NULL);
  gtk_widget_show (frame261);
  gtk_table_attach (GTK_TABLE (table31), frame261, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment382 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment382);
  gtk_container_add (GTK_CONTAINER (frame261), alignment382);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment382), 0, 0, 12, 0);

  alignment383 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment383);
  gtk_container_add (GTK_CONTAINER (alignment382), alignment383);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment383), 8, 8, 5, 0);

  hbox284 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox284);
  gtk_container_add (GTK_CONTAINER (alignment383), hbox284);

  label11220 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11220);
  gtk_box_pack_start (GTK_BOX (hbox284), label11220, FALSE, FALSE, 0);

  scrolledwindow150 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow150);
  gtk_box_pack_start (GTK_BOX (hbox284), scrolledwindow150, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow150), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow150), GTK_SHADOW_IN);

  textview112 = gtk_text_view_new ();
  gtk_widget_show (textview112);
  gtk_container_add (GTK_CONTAINER (scrolledwindow150), textview112);

  frame262 = gtk_frame_new (NULL);
  gtk_widget_show (frame262);
  gtk_box_pack_start (GTK_BOX (hbox284), frame262, FALSE, FALSE, 0);

  alignment384 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment384);
  gtk_container_add (GTK_CONTAINER (frame262), alignment384);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment384), 0, 0, 12, 0);

  hbox285 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox285);
  gtk_container_add (GTK_CONTAINER (alignment384), hbox285);

  entry67 = gtk_entry_new ();
  gtk_widget_show (entry67);
  gtk_box_pack_start (GTK_BOX (hbox285), entry67, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry67, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry67), 8226);

  button147 = gtk_button_new ();
  gtk_widget_show (button147);
  gtk_box_pack_start (GTK_BOX (hbox285), button147, FALSE, FALSE, 0);

  image176 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image176);
  gtk_container_add (GTK_CONTAINER (button147), image176);

  label11221 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11221);
  gtk_frame_set_label_widget (GTK_FRAME (frame262), label11221);
  gtk_label_set_use_markup (GTK_LABEL (label11221), TRUE);

  label11222 = gtk_label_new (_("AX12"));
  gtk_widget_show (label11222);
  gtk_frame_set_label_widget (GTK_FRAME (frame261), label11222);
  gtk_label_set_use_markup (GTK_LABEL (label11222), TRUE);

  frame263 = gtk_frame_new (NULL);
  gtk_widget_show (frame263);
  gtk_table_attach (GTK_TABLE (table31), frame263, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment385 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment385);
  gtk_container_add (GTK_CONTAINER (frame263), alignment385);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment385), 0, 0, 12, 0);

  hbox286 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox286);
  gtk_container_add (GTK_CONTAINER (alignment385), hbox286);
  gtk_container_set_border_width (GTK_CONTAINER (hbox286), 5);

  frame264 = gtk_frame_new (NULL);
  gtk_widget_show (frame264);
  gtk_box_pack_start (GTK_BOX (hbox286), frame264, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame264, 17, -1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame264), GTK_SHADOW_NONE);

  alignment386 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment386);
  gtk_container_add (GTK_CONTAINER (frame264), alignment386);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment386), 0, 0, 12, 0);

  scrolledwindow151 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow151);
  gtk_container_add (GTK_CONTAINER (alignment386), scrolledwindow151);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow151), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow151), GTK_SHADOW_IN);

  textview113 = gtk_text_view_new ();
  gtk_widget_show (textview113);
  gtk_container_add (GTK_CONTAINER (scrolledwindow151), textview113);

  label11223 = gtk_label_new (_("N\302\260 ADC"));
  gtk_widget_show (label11223);
  gtk_frame_set_label_widget (GTK_FRAME (frame264), label11223);
  gtk_label_set_use_markup (GTK_LABEL (label11223), TRUE);

  frame265 = gtk_frame_new (NULL);
  gtk_widget_show (frame265);
  gtk_box_pack_start (GTK_BOX (hbox286), frame265, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame265), 0.01, 0.5);

  alignment387 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment387);
  gtk_container_add (GTK_CONTAINER (frame265), alignment387);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment387), 0, 0, 12, 0);

  hbox287 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox287);
  gtk_container_add (GTK_CONTAINER (alignment387), hbox287);

  entry68 = gtk_entry_new ();
  gtk_widget_show (entry68);
  gtk_box_pack_start (GTK_BOX (hbox287), entry68, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry68, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry68), 8226);

  button148 = gtk_button_new ();
  gtk_widget_show (button148);
  gtk_box_pack_start (GTK_BOX (hbox287), button148, FALSE, TRUE, 0);

  image177 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image177);
  gtk_container_add (GTK_CONTAINER (button148), image177);

  label11224 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11224);
  gtk_frame_set_label_widget (GTK_FRAME (frame265), label11224);
  gtk_widget_set_size_request (label11224, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11224), TRUE);

  label11225 = gtk_label_new (_("ADC"));
  gtk_widget_show (label11225);
  gtk_frame_set_label_widget (GTK_FRAME (frame263), label11225);
  gtk_label_set_use_markup (GTK_LABEL (label11225), TRUE);

  frame266 = gtk_frame_new (NULL);
  gtk_widget_show (frame266);
  gtk_table_attach (GTK_TABLE (table31), frame266, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment388 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment388);
  gtk_container_add (GTK_CONTAINER (frame266), alignment388);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment388), 0, 0, 12, 0);

  alignment389 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment389);
  gtk_container_add (GTK_CONTAINER (alignment388), alignment389);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment389), 0, 0, 12, 0);

  hbox288 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox288);
  gtk_container_add (GTK_CONTAINER (alignment389), hbox288);

  label11226 = gtk_label_new (_("Valeur       "));
  gtk_widget_show (label11226);
  gtk_box_pack_start (GTK_BOX (hbox288), label11226, FALSE, FALSE, 0);

  scrolledwindow152 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow152);
  gtk_box_pack_start (GTK_BOX (hbox288), scrolledwindow152, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow152), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow152), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow152), GTK_SHADOW_IN);

  textview114 = gtk_text_view_new ();
  gtk_widget_show (textview114);
  gtk_container_add (GTK_CONTAINER (scrolledwindow152), textview114);
  gtk_widget_set_size_request (textview114, -1, 8);

  label11227 = gtk_label_new (_("PWM"));
  gtk_widget_show (label11227);
  gtk_frame_set_label_widget (GTK_FRAME (frame266), label11227);
  gtk_label_set_use_markup (GTK_LABEL (label11227), TRUE);

  label333 = gtk_label_new (_("Actionneurs"));
  gtk_widget_show (label333);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (cei_ong_all), gtk_notebook_get_nth_page (GTK_NOTEBOOK (cei_ong_all), 0), label333);

  vbox110 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox110);
  gtk_container_add (GTK_CONTAINER (cei_ong_all), vbox110);

  table33 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table33);
  gtk_box_pack_start (GTK_BOX (vbox110), table33, TRUE, TRUE, 0);

  scrolledwindow155 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow155);
  gtk_table_attach (GTK_TABLE (table33), scrolledwindow155, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow155), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow155), GTK_SHADOW_IN);

  viewport8 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport8);
  gtk_container_add (GTK_CONTAINER (scrolledwindow155), viewport8);
  gtk_widget_set_size_request (viewport8, 42, 196);

  vbox111 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox111);
  gtk_container_add (GTK_CONTAINER (viewport8), vbox111);

  hbox307 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox307);
  gtk_box_pack_start (GTK_BOX (vbox111), hbox307, TRUE, TRUE, 0);

  hbox308 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox308);
  gtk_box_pack_start (GTK_BOX (hbox307), hbox308, TRUE, TRUE, 0);

  label11256 = gtk_label_new (_("Error"));
  gtk_widget_show (label11256);
  gtk_box_pack_start (GTK_BOX (hbox308), label11256, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11256), GTK_JUSTIFY_CENTER);

  image186 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image186);
  gtk_box_pack_end (GTK_BOX (hbox308), image186, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image186, FALSE);

  hbox309 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox309);
  gtk_box_pack_start (GTK_BOX (vbox111), hbox309, TRUE, TRUE, 0);

  label11257 = gtk_label_new (_("User"));
  gtk_widget_show (label11257);
  gtk_box_pack_start (GTK_BOX (hbox309), label11257, TRUE, FALSE, 0);

  image187 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image187);
  gtk_box_pack_end (GTK_BOX (hbox309), image187, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image187, FALSE);

  hbox310 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox310);
  gtk_box_pack_start (GTK_BOX (vbox111), hbox310, FALSE, FALSE, 0);

  label11258 = gtk_label_new (_("User"));
  gtk_widget_show (label11258);
  gtk_box_pack_start (GTK_BOX (hbox310), label11258, TRUE, FALSE, 0);

  image188 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image188);
  gtk_box_pack_end (GTK_BOX (hbox310), image188, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image188, FALSE);

  hbox311 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox311);
  gtk_box_pack_start (GTK_BOX (vbox111), hbox311, TRUE, TRUE, 0);

  label11259 = gtk_label_new (_("Uart"));
  gtk_widget_show (label11259);
  gtk_box_pack_start (GTK_BOX (hbox311), label11259, TRUE, FALSE, 0);

  image189 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image189);
  gtk_box_pack_end (GTK_BOX (hbox311), image189, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image189, FALSE);

  hbox312 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox312);
  gtk_box_pack_start (GTK_BOX (vbox111), hbox312, TRUE, TRUE, 0);

  label11260 = gtk_label_new (_("Can"));
  gtk_widget_show (label11260);
  gtk_box_pack_start (GTK_BOX (hbox312), label11260, TRUE, FALSE, 0);

  image190 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image190);
  gtk_box_pack_end (GTK_BOX (hbox312), image190, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image190, FALSE);

  hbox313 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox313);
  gtk_box_pack_start (GTK_BOX (vbox111), hbox313, TRUE, TRUE, 0);

  label11261 = gtk_label_new (_("Run"));
  gtk_widget_show (label11261);
  gtk_box_pack_start (GTK_BOX (hbox313), label11261, TRUE, FALSE, 0);

  image191 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image191);
  gtk_box_pack_end (GTK_BOX (hbox313), image191, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image191, FALSE);

  frame281 = gtk_frame_new (NULL);
  gtk_widget_show (frame281);
  gtk_table_attach (GTK_TABLE (table33), frame281, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame281, 54, 43);

  alignment410 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment410);
  gtk_container_add (GTK_CONTAINER (frame281), alignment410);
  gtk_widget_set_size_request (alignment410, -1, 162);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment410), 0, 0, 12, 0);

  image192 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image192);
  gtk_container_add (GTK_CONTAINER (alignment410), image192);
  gtk_widget_set_size_request (image192, -1, 330);
  gtk_widget_set_sensitive (image192, FALSE);

  label11262 = gtk_label_new (_("Carte en\n fonctionnement"));
  gtk_widget_show (label11262);
  gtk_frame_set_label_widget (GTK_FRAME (frame281), label11262);
  gtk_label_set_use_markup (GTK_LABEL (label11262), TRUE);
  gtk_label_set_justify (GTK_LABEL (label11262), GTK_JUSTIFY_CENTER);

  hbuttonbox6 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox6);
  gtk_table_attach (GTK_TABLE (table33), hbuttonbox6, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hbuttonbox6, 449, 2);

  cei_pro_button_bt1 = gtk_button_new_with_mnemonic (_("BT1"));
  gtk_widget_show (cei_pro_button_bt1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox6), cei_pro_button_bt1);
  gtk_container_set_border_width (GTK_CONTAINER (cei_pro_button_bt1), 11);
  GTK_WIDGET_SET_FLAGS (cei_pro_button_bt1, GTK_CAN_DEFAULT);

  cei_pro_button_bt2 = gtk_button_new_with_mnemonic (_("BT2"));
  gtk_widget_show (cei_pro_button_bt2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox6), cei_pro_button_bt2);
  gtk_container_set_border_width (GTK_CONTAINER (cei_pro_button_bt2), 11);
  GTK_WIDGET_SET_FLAGS (cei_pro_button_bt2, GTK_CAN_DEFAULT);

  cei_pro_button_bt3 = gtk_button_new_with_mnemonic (_("BT3"));
  gtk_widget_show (cei_pro_button_bt3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox6), cei_pro_button_bt3);
  gtk_container_set_border_width (GTK_CONTAINER (cei_pro_button_bt3), 11);
  GTK_WIDGET_SET_FLAGS (cei_pro_button_bt3, GTK_CAN_DEFAULT);

  cei_pro_button_bt4 = gtk_button_new_with_mnemonic (_("BT4"));
  gtk_widget_show (cei_pro_button_bt4);
  gtk_container_add (GTK_CONTAINER (hbuttonbox6), cei_pro_button_bt4);
  gtk_container_set_border_width (GTK_CONTAINER (cei_pro_button_bt4), 11);
  GTK_WIDGET_SET_FLAGS (cei_pro_button_bt4, GTK_CAN_DEFAULT);

  button158 = gtk_button_new_with_mnemonic (_("Reset"));
  gtk_widget_show (button158);
  gtk_container_add (GTK_CONTAINER (hbuttonbox6), button158);
  gtk_container_set_border_width (GTK_CONTAINER (button158), 11);
  GTK_WIDGET_SET_FLAGS (button158, GTK_CAN_DEFAULT);

  image193 = create_pixmap (Fenetre, "carte.png");
  gtk_widget_show (image193);
  gtk_table_attach (GTK_TABLE (table33), image193, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);

  vbox112 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox112);
  gtk_box_pack_start (GTK_BOX (vbox110), vbox112, TRUE, TRUE, 0);

  table34 = gtk_table_new (3, 3, TRUE);
  gtk_widget_show (table34);
  gtk_box_pack_start (GTK_BOX (vbox112), table34, TRUE, TRUE, 0);
  gtk_widget_set_size_request (table34, 477, 249);

  vbox113 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox113);
  gtk_table_attach (GTK_TABLE (table34), vbox113, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame282 = gtk_frame_new (NULL);
  gtk_widget_show (frame282);
  gtk_box_pack_start (GTK_BOX (vbox113), frame282, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame282, 41, 15);

  alignment411 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment411);
  gtk_container_add (GTK_CONTAINER (frame282), alignment411);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment411), 0, 0, 12, 0);

  hbox314 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox314);
  gtk_container_add (GTK_CONTAINER (alignment411), hbox314);

  label11263 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11263);
  gtk_box_pack_start (GTK_BOX (hbox314), label11263, FALSE, FALSE, 0);

  scrolledwindow156 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow156);
  gtk_box_pack_start (GTK_BOX (hbox314), scrolledwindow156, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow156), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow156), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow156), GTK_SHADOW_IN);

  textview116 = gtk_text_view_new ();
  gtk_widget_show (textview116);
  gtk_container_add (GTK_CONTAINER (scrolledwindow156), textview116);
  gtk_widget_set_size_request (textview116, -1, 8);

  label11264 = gtk_label_new (_("Step Motor"));
  gtk_widget_show (label11264);
  gtk_frame_set_label_widget (GTK_FRAME (frame282), label11264);
  gtk_label_set_use_markup (GTK_LABEL (label11264), TRUE);

  vbox114 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox114);
  gtk_table_attach (GTK_TABLE (table34), vbox114, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame283 = gtk_frame_new (NULL);
  gtk_widget_show (frame283);
  gtk_box_pack_start (GTK_BOX (vbox114), frame283, TRUE, TRUE, 0);

  alignment412 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment412);
  gtk_container_add (GTK_CONTAINER (frame283), alignment412);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment412), 0, 0, 12, 0);

  alignment413 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment413);
  gtk_container_add (GTK_CONTAINER (alignment412), alignment413);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment413), 8, 8, 5, 0);

  hbox315 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox315);
  gtk_container_add (GTK_CONTAINER (alignment413), hbox315);

  label11265 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11265);
  gtk_box_pack_start (GTK_BOX (hbox315), label11265, FALSE, FALSE, 0);

  scrolledwindow157 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow157);
  gtk_box_pack_start (GTK_BOX (hbox315), scrolledwindow157, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow157), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow157), GTK_SHADOW_IN);

  textview117 = gtk_text_view_new ();
  gtk_widget_show (textview117);
  gtk_container_add (GTK_CONTAINER (scrolledwindow157), textview117);

  frame284 = gtk_frame_new (NULL);
  gtk_widget_show (frame284);
  gtk_box_pack_start (GTK_BOX (hbox315), frame284, FALSE, FALSE, 0);

  alignment414 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment414);
  gtk_container_add (GTK_CONTAINER (frame284), alignment414);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment414), 0, 0, 12, 0);

  hbox316 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox316);
  gtk_container_add (GTK_CONTAINER (alignment414), hbox316);

  entry69 = gtk_entry_new ();
  gtk_widget_show (entry69);
  gtk_box_pack_start (GTK_BOX (hbox316), entry69, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry69, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry69), 8226);

  button159 = gtk_button_new ();
  gtk_widget_show (button159);
  gtk_box_pack_start (GTK_BOX (hbox316), button159, FALSE, FALSE, 0);

  image194 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image194);
  gtk_container_add (GTK_CONTAINER (button159), image194);

  label11266 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11266);
  gtk_frame_set_label_widget (GTK_FRAME (frame284), label11266);
  gtk_label_set_use_markup (GTK_LABEL (label11266), TRUE);

  label11267 = gtk_label_new (_("Extern It"));
  gtk_widget_show (label11267);
  gtk_frame_set_label_widget (GTK_FRAME (frame283), label11267);
  gtk_label_set_use_markup (GTK_LABEL (label11267), TRUE);

  frame285 = gtk_frame_new (NULL);
  gtk_widget_show (frame285);
  gtk_table_attach (GTK_TABLE (table34), frame285, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame285, 83, 9);

  alignment415 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment415);
  gtk_container_add (GTK_CONTAINER (frame285), alignment415);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment415), 0, 0, 12, 0);

  alignment416 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment416);
  gtk_container_add (GTK_CONTAINER (alignment415), alignment416);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment416), 0, 0, 12, 0);

  hbox317 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox317);
  gtk_container_add (GTK_CONTAINER (alignment416), hbox317);

  label11268 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11268);
  gtk_box_pack_start (GTK_BOX (hbox317), label11268, FALSE, FALSE, 0);

  scrolledwindow158 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow158);
  gtk_box_pack_start (GTK_BOX (hbox317), scrolledwindow158, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow158), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow158), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow158), GTK_SHADOW_IN);

  textview118 = gtk_text_view_new ();
  gtk_widget_show (textview118);
  gtk_container_add (GTK_CONTAINER (scrolledwindow158), textview118);
  gtk_widget_set_size_request (textview118, -1, 8);

  label11269 = gtk_label_new (_("Servo"));
  gtk_widget_show (label11269);
  gtk_frame_set_label_widget (GTK_FRAME (frame285), label11269);
  gtk_label_set_use_markup (GTK_LABEL (label11269), TRUE);

  frame286 = gtk_frame_new (NULL);
  gtk_widget_show (frame286);
  gtk_table_attach (GTK_TABLE (table34), frame286, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment417 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment417);
  gtk_container_add (GTK_CONTAINER (frame286), alignment417);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment417), 8, 8, 5, 0);

  hbox318 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox318);
  gtk_container_add (GTK_CONTAINER (alignment417), hbox318);

  label11270 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11270);
  gtk_box_pack_start (GTK_BOX (hbox318), label11270, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label11270, 46, -1);

  scrolledwindow159 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow159);
  gtk_box_pack_start (GTK_BOX (hbox318), scrolledwindow159, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow159), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow159), GTK_SHADOW_IN);

  textview119 = gtk_text_view_new ();
  gtk_widget_show (textview119);
  gtk_container_add (GTK_CONTAINER (scrolledwindow159), textview119);

  frame287 = gtk_frame_new (NULL);
  gtk_widget_show (frame287);
  gtk_box_pack_start (GTK_BOX (hbox318), frame287, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame287), 0.01, 0.5);

  alignment418 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment418);
  gtk_container_add (GTK_CONTAINER (frame287), alignment418);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment418), 0, 0, 12, 0);

  hbox319 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox319);
  gtk_container_add (GTK_CONTAINER (alignment418), hbox319);

  entry70 = gtk_entry_new ();
  gtk_widget_show (entry70);
  gtk_box_pack_start (GTK_BOX (hbox319), entry70, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry70, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry70), 8226);

  button160 = gtk_button_new ();
  gtk_widget_show (button160);
  gtk_box_pack_start (GTK_BOX (hbox319), button160, FALSE, FALSE, 0);

  image195 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image195);
  gtk_container_add (GTK_CONTAINER (button160), image195);

  label11271 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11271);
  gtk_frame_set_label_widget (GTK_FRAME (frame287), label11271);
  gtk_widget_set_size_request (label11271, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11271), TRUE);

  label11272 = gtk_label_new (_("QEI"));
  gtk_widget_show (label11272);
  gtk_frame_set_label_widget (GTK_FRAME (frame286), label11272);
  gtk_label_set_use_markup (GTK_LABEL (label11272), TRUE);

  frame288 = gtk_frame_new (NULL);
  gtk_widget_show (frame288);
  gtk_table_attach (GTK_TABLE (table34), frame288, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment419 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment419);
  gtk_container_add (GTK_CONTAINER (frame288), alignment419);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment419), 0, 0, 12, 0);

  alignment420 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment420);
  gtk_container_add (GTK_CONTAINER (alignment419), alignment420);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment420), 8, 8, 5, 0);

  hbox320 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox320);
  gtk_container_add (GTK_CONTAINER (alignment420), hbox320);

  label11273 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11273);
  gtk_box_pack_start (GTK_BOX (hbox320), label11273, FALSE, FALSE, 0);

  scrolledwindow160 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow160);
  gtk_box_pack_start (GTK_BOX (hbox320), scrolledwindow160, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow160), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow160), GTK_SHADOW_IN);

  textview120 = gtk_text_view_new ();
  gtk_widget_show (textview120);
  gtk_container_add (GTK_CONTAINER (scrolledwindow160), textview120);

  frame289 = gtk_frame_new (NULL);
  gtk_widget_show (frame289);
  gtk_box_pack_start (GTK_BOX (hbox320), frame289, FALSE, FALSE, 0);

  alignment421 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment421);
  gtk_container_add (GTK_CONTAINER (frame289), alignment421);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment421), 0, 0, 12, 0);

  hbox321 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox321);
  gtk_container_add (GTK_CONTAINER (alignment421), hbox321);

  entry71 = gtk_entry_new ();
  gtk_widget_show (entry71);
  gtk_box_pack_start (GTK_BOX (hbox321), entry71, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry71, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry71), 8226);

  button161 = gtk_button_new ();
  gtk_widget_show (button161);
  gtk_box_pack_start (GTK_BOX (hbox321), button161, FALSE, FALSE, 0);

  image196 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image196);
  gtk_container_add (GTK_CONTAINER (button161), image196);

  label11274 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11274);
  gtk_frame_set_label_widget (GTK_FRAME (frame289), label11274);
  gtk_label_set_use_markup (GTK_LABEL (label11274), TRUE);

  label11275 = gtk_label_new (_("DCM"));
  gtk_widget_show (label11275);
  gtk_frame_set_label_widget (GTK_FRAME (frame288), label11275);
  gtk_label_set_use_markup (GTK_LABEL (label11275), TRUE);

  frame290 = gtk_frame_new (NULL);
  gtk_widget_show (frame290);
  gtk_table_attach (GTK_TABLE (table34), frame290, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame290), GTK_SHADOW_NONE);

  alignment422 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment422);
  gtk_container_add (GTK_CONTAINER (frame290), alignment422);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment422), 0, 0, 12, 0);

  drawingarea11 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea11);
  gtk_container_add (GTK_CONTAINER (alignment422), drawingarea11);

  label11276 = gtk_label_new ("");
  gtk_widget_show (label11276);
  gtk_frame_set_label_widget (GTK_FRAME (frame290), label11276);
  gtk_label_set_use_markup (GTK_LABEL (label11276), TRUE);

  frame291 = gtk_frame_new (NULL);
  gtk_widget_show (frame291);
  gtk_table_attach (GTK_TABLE (table34), frame291, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment423 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment423);
  gtk_container_add (GTK_CONTAINER (frame291), alignment423);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment423), 0, 0, 12, 0);

  alignment424 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment424);
  gtk_container_add (GTK_CONTAINER (alignment423), alignment424);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment424), 8, 8, 5, 0);

  hbox322 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox322);
  gtk_container_add (GTK_CONTAINER (alignment424), hbox322);

  label11277 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11277);
  gtk_box_pack_start (GTK_BOX (hbox322), label11277, FALSE, FALSE, 0);

  scrolledwindow161 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow161);
  gtk_box_pack_start (GTK_BOX (hbox322), scrolledwindow161, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow161), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow161), GTK_SHADOW_IN);

  textview121 = gtk_text_view_new ();
  gtk_widget_show (textview121);
  gtk_container_add (GTK_CONTAINER (scrolledwindow161), textview121);

  frame292 = gtk_frame_new (NULL);
  gtk_widget_show (frame292);
  gtk_box_pack_start (GTK_BOX (hbox322), frame292, FALSE, FALSE, 0);

  alignment425 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment425);
  gtk_container_add (GTK_CONTAINER (frame292), alignment425);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment425), 0, 0, 12, 0);

  hbox323 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox323);
  gtk_container_add (GTK_CONTAINER (alignment425), hbox323);

  entry72 = gtk_entry_new ();
  gtk_widget_show (entry72);
  gtk_box_pack_start (GTK_BOX (hbox323), entry72, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry72, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry72), 8226);

  button162 = gtk_button_new ();
  gtk_widget_show (button162);
  gtk_box_pack_start (GTK_BOX (hbox323), button162, FALSE, FALSE, 0);

  image197 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image197);
  gtk_container_add (GTK_CONTAINER (button162), image197);

  label11278 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11278);
  gtk_frame_set_label_widget (GTK_FRAME (frame292), label11278);
  gtk_label_set_use_markup (GTK_LABEL (label11278), TRUE);

  label11279 = gtk_label_new (_("AX12"));
  gtk_widget_show (label11279);
  gtk_frame_set_label_widget (GTK_FRAME (frame291), label11279);
  gtk_label_set_use_markup (GTK_LABEL (label11279), TRUE);

  frame293 = gtk_frame_new (NULL);
  gtk_widget_show (frame293);
  gtk_table_attach (GTK_TABLE (table34), frame293, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment426 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment426);
  gtk_container_add (GTK_CONTAINER (frame293), alignment426);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment426), 0, 0, 12, 0);

  hbox324 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox324);
  gtk_container_add (GTK_CONTAINER (alignment426), hbox324);
  gtk_container_set_border_width (GTK_CONTAINER (hbox324), 5);

  frame294 = gtk_frame_new (NULL);
  gtk_widget_show (frame294);
  gtk_box_pack_start (GTK_BOX (hbox324), frame294, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame294, 17, -1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame294), GTK_SHADOW_NONE);

  alignment427 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment427);
  gtk_container_add (GTK_CONTAINER (frame294), alignment427);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment427), 0, 0, 12, 0);

  scrolledwindow162 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow162);
  gtk_container_add (GTK_CONTAINER (alignment427), scrolledwindow162);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow162), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow162), GTK_SHADOW_IN);

  textview122 = gtk_text_view_new ();
  gtk_widget_show (textview122);
  gtk_container_add (GTK_CONTAINER (scrolledwindow162), textview122);

  label11280 = gtk_label_new (_("N\302\260 ADC"));
  gtk_widget_show (label11280);
  gtk_frame_set_label_widget (GTK_FRAME (frame294), label11280);
  gtk_label_set_use_markup (GTK_LABEL (label11280), TRUE);

  frame295 = gtk_frame_new (NULL);
  gtk_widget_show (frame295);
  gtk_box_pack_start (GTK_BOX (hbox324), frame295, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame295), 0.01, 0.5);

  alignment428 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment428);
  gtk_container_add (GTK_CONTAINER (frame295), alignment428);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment428), 0, 0, 12, 0);

  hbox325 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox325);
  gtk_container_add (GTK_CONTAINER (alignment428), hbox325);

  entry73 = gtk_entry_new ();
  gtk_widget_show (entry73);
  gtk_box_pack_start (GTK_BOX (hbox325), entry73, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry73, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry73), 8226);

  button163 = gtk_button_new ();
  gtk_widget_show (button163);
  gtk_box_pack_start (GTK_BOX (hbox325), button163, FALSE, TRUE, 0);

  image198 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image198);
  gtk_container_add (GTK_CONTAINER (button163), image198);

  label11281 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11281);
  gtk_frame_set_label_widget (GTK_FRAME (frame295), label11281);
  gtk_widget_set_size_request (label11281, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11281), TRUE);

  label11282 = gtk_label_new (_("ADC"));
  gtk_widget_show (label11282);
  gtk_frame_set_label_widget (GTK_FRAME (frame293), label11282);
  gtk_label_set_use_markup (GTK_LABEL (label11282), TRUE);

  frame296 = gtk_frame_new (NULL);
  gtk_widget_show (frame296);
  gtk_table_attach (GTK_TABLE (table34), frame296, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment429 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment429);
  gtk_container_add (GTK_CONTAINER (frame296), alignment429);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment429), 0, 0, 12, 0);

  alignment430 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment430);
  gtk_container_add (GTK_CONTAINER (alignment429), alignment430);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment430), 0, 0, 12, 0);

  hbox326 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox326);
  gtk_container_add (GTK_CONTAINER (alignment430), hbox326);

  label11283 = gtk_label_new (_("Valeur       "));
  gtk_widget_show (label11283);
  gtk_box_pack_start (GTK_BOX (hbox326), label11283, FALSE, FALSE, 0);

  scrolledwindow163 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow163);
  gtk_box_pack_start (GTK_BOX (hbox326), scrolledwindow163, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow163), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow163), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow163), GTK_SHADOW_IN);

  textview123 = gtk_text_view_new ();
  gtk_widget_show (textview123);
  gtk_container_add (GTK_CONTAINER (scrolledwindow163), textview123);
  gtk_widget_set_size_request (textview123, -1, 8);

  label11284 = gtk_label_new (_("PWM"));
  gtk_widget_show (label11284);
  gtk_frame_set_label_widget (GTK_FRAME (frame296), label11284);
  gtk_label_set_use_markup (GTK_LABEL (label11284), TRUE);

  label12333 = gtk_label_new (_("Propulsion"));
  gtk_widget_show (label12333);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (cei_ong_all), gtk_notebook_get_nth_page (GTK_NOTEBOOK (cei_ong_all), 1), label12333);

  vbox115 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox115);
  gtk_container_add (GTK_CONTAINER (cei_ong_all), vbox115);

  table35 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table35);
  gtk_box_pack_start (GTK_BOX (vbox115), table35, TRUE, TRUE, 0);

  scrolledwindow164 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow164);
  gtk_table_attach (GTK_TABLE (table35), scrolledwindow164, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow164), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow164), GTK_SHADOW_IN);

  viewport9 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport9);
  gtk_container_add (GTK_CONTAINER (scrolledwindow164), viewport9);
  gtk_widget_set_size_request (viewport9, 42, 196);

  vbox116 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox116);
  gtk_container_add (GTK_CONTAINER (viewport9), vbox116);

  hbox327 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox327);
  gtk_box_pack_start (GTK_BOX (vbox116), hbox327, TRUE, TRUE, 0);

  hbox328 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox328);
  gtk_box_pack_start (GTK_BOX (hbox327), hbox328, TRUE, TRUE, 0);

  label11285 = gtk_label_new (_("Error"));
  gtk_widget_show (label11285);
  gtk_box_pack_start (GTK_BOX (hbox328), label11285, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11285), GTK_JUSTIFY_CENTER);

  image199 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image199);
  gtk_box_pack_end (GTK_BOX (hbox328), image199, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image199, FALSE);

  hbox329 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox329);
  gtk_box_pack_start (GTK_BOX (vbox116), hbox329, TRUE, TRUE, 0);

  label11286 = gtk_label_new (_("User"));
  gtk_widget_show (label11286);
  gtk_box_pack_start (GTK_BOX (hbox329), label11286, TRUE, FALSE, 0);

  image200 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image200);
  gtk_box_pack_end (GTK_BOX (hbox329), image200, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image200, FALSE);

  hbox330 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox330);
  gtk_box_pack_start (GTK_BOX (vbox116), hbox330, FALSE, FALSE, 0);

  label11287 = gtk_label_new (_("User"));
  gtk_widget_show (label11287);
  gtk_box_pack_start (GTK_BOX (hbox330), label11287, TRUE, FALSE, 0);

  image201 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image201);
  gtk_box_pack_end (GTK_BOX (hbox330), image201, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image201, FALSE);

  hbox331 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox331);
  gtk_box_pack_start (GTK_BOX (vbox116), hbox331, TRUE, TRUE, 0);

  label11288 = gtk_label_new (_("Uart"));
  gtk_widget_show (label11288);
  gtk_box_pack_start (GTK_BOX (hbox331), label11288, TRUE, FALSE, 0);

  image202 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image202);
  gtk_box_pack_end (GTK_BOX (hbox331), image202, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image202, FALSE);

  hbox332 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox332);
  gtk_box_pack_start (GTK_BOX (vbox116), hbox332, TRUE, TRUE, 0);

  label11289 = gtk_label_new (_("Can"));
  gtk_widget_show (label11289);
  gtk_box_pack_start (GTK_BOX (hbox332), label11289, TRUE, FALSE, 0);

  image203 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image203);
  gtk_box_pack_end (GTK_BOX (hbox332), image203, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image203, FALSE);

  hbox333 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox333);
  gtk_box_pack_start (GTK_BOX (vbox116), hbox333, TRUE, TRUE, 0);

  label11290 = gtk_label_new (_("Run"));
  gtk_widget_show (label11290);
  gtk_box_pack_start (GTK_BOX (hbox333), label11290, TRUE, FALSE, 0);

  image204 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image204);
  gtk_box_pack_end (GTK_BOX (hbox333), image204, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image204, FALSE);

  frame297 = gtk_frame_new (NULL);
  gtk_widget_show (frame297);
  gtk_table_attach (GTK_TABLE (table35), frame297, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame297, 54, 43);

  alignment431 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment431);
  gtk_container_add (GTK_CONTAINER (frame297), alignment431);
  gtk_widget_set_size_request (alignment431, -1, 162);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment431), 0, 0, 12, 0);

  image205 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image205);
  gtk_container_add (GTK_CONTAINER (alignment431), image205);
  gtk_widget_set_size_request (image205, -1, 330);
  gtk_widget_set_sensitive (image205, FALSE);

  label11291 = gtk_label_new (_("Carte en\n fonctionnement"));
  gtk_widget_show (label11291);
  gtk_frame_set_label_widget (GTK_FRAME (frame297), label11291);
  gtk_label_set_use_markup (GTK_LABEL (label11291), TRUE);
  gtk_label_set_justify (GTK_LABEL (label11291), GTK_JUSTIFY_CENTER);

  hbuttonbox7 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox7);
  gtk_table_attach (GTK_TABLE (table35), hbuttonbox7, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hbuttonbox7, 449, 2);

  cei_stra_button_bt1 = gtk_button_new_with_mnemonic (_("BT1"));
  gtk_widget_show (cei_stra_button_bt1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox7), cei_stra_button_bt1);
  gtk_container_set_border_width (GTK_CONTAINER (cei_stra_button_bt1), 11);
  GTK_WIDGET_SET_FLAGS (cei_stra_button_bt1, GTK_CAN_DEFAULT);

  button165 = gtk_button_new_with_mnemonic (_("BT2"));
  gtk_widget_show (button165);
  gtk_container_add (GTK_CONTAINER (hbuttonbox7), button165);
  gtk_container_set_border_width (GTK_CONTAINER (button165), 11);
  GTK_WIDGET_SET_FLAGS (button165, GTK_CAN_DEFAULT);

  cei_stra_button_bt3 = gtk_button_new_with_mnemonic (_("BT3"));
  gtk_widget_show (cei_stra_button_bt3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox7), cei_stra_button_bt3);
  gtk_container_set_border_width (GTK_CONTAINER (cei_stra_button_bt3), 11);
  GTK_WIDGET_SET_FLAGS (cei_stra_button_bt3, GTK_CAN_DEFAULT);

  cei_stra_button_bt4 = gtk_button_new_with_mnemonic (_("BT4"));
  gtk_widget_show (cei_stra_button_bt4);
  gtk_container_add (GTK_CONTAINER (hbuttonbox7), cei_stra_button_bt4);
  gtk_container_set_border_width (GTK_CONTAINER (cei_stra_button_bt4), 11);
  GTK_WIDGET_SET_FLAGS (cei_stra_button_bt4, GTK_CAN_DEFAULT);

  cei_stra_button_reset = gtk_button_new_with_mnemonic (_("Reset"));
  gtk_widget_show (cei_stra_button_reset);
  gtk_container_add (GTK_CONTAINER (hbuttonbox7), cei_stra_button_reset);
  gtk_container_set_border_width (GTK_CONTAINER (cei_stra_button_reset), 11);
  GTK_WIDGET_SET_FLAGS (cei_stra_button_reset, GTK_CAN_DEFAULT);

  image206 = create_pixmap (Fenetre, "carte.png");
  gtk_widget_show (image206);
  gtk_table_attach (GTK_TABLE (table35), image206, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);

  vbox117 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox117);
  gtk_box_pack_start (GTK_BOX (vbox115), vbox117, TRUE, TRUE, 0);

  table36 = gtk_table_new (3, 3, TRUE);
  gtk_widget_show (table36);
  gtk_box_pack_start (GTK_BOX (vbox117), table36, TRUE, TRUE, 0);
  gtk_widget_set_size_request (table36, 477, 249);

  vbox118 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox118);
  gtk_table_attach (GTK_TABLE (table36), vbox118, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame298 = gtk_frame_new (NULL);
  gtk_widget_show (frame298);
  gtk_box_pack_start (GTK_BOX (vbox118), frame298, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame298, 41, 15);

  alignment432 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment432);
  gtk_container_add (GTK_CONTAINER (frame298), alignment432);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment432), 0, 0, 12, 0);

  hbox334 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox334);
  gtk_container_add (GTK_CONTAINER (alignment432), hbox334);

  label11292 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11292);
  gtk_box_pack_start (GTK_BOX (hbox334), label11292, FALSE, FALSE, 0);

  scrolledwindow165 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow165);
  gtk_box_pack_start (GTK_BOX (hbox334), scrolledwindow165, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow165), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow165), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow165), GTK_SHADOW_IN);

  textview124 = gtk_text_view_new ();
  gtk_widget_show (textview124);
  gtk_container_add (GTK_CONTAINER (scrolledwindow165), textview124);
  gtk_widget_set_size_request (textview124, -1, 8);

  label11293 = gtk_label_new (_("Step Motor"));
  gtk_widget_show (label11293);
  gtk_frame_set_label_widget (GTK_FRAME (frame298), label11293);
  gtk_label_set_use_markup (GTK_LABEL (label11293), TRUE);

  vbox119 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox119);
  gtk_table_attach (GTK_TABLE (table36), vbox119, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame299 = gtk_frame_new (NULL);
  gtk_widget_show (frame299);
  gtk_box_pack_start (GTK_BOX (vbox119), frame299, TRUE, TRUE, 0);

  alignment433 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment433);
  gtk_container_add (GTK_CONTAINER (frame299), alignment433);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment433), 0, 0, 12, 0);

  alignment434 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment434);
  gtk_container_add (GTK_CONTAINER (alignment433), alignment434);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment434), 8, 8, 5, 0);

  hbox335 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox335);
  gtk_container_add (GTK_CONTAINER (alignment434), hbox335);

  label11294 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11294);
  gtk_box_pack_start (GTK_BOX (hbox335), label11294, FALSE, FALSE, 0);

  scrolledwindow166 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow166);
  gtk_box_pack_start (GTK_BOX (hbox335), scrolledwindow166, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow166), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow166), GTK_SHADOW_IN);

  textview125 = gtk_text_view_new ();
  gtk_widget_show (textview125);
  gtk_container_add (GTK_CONTAINER (scrolledwindow166), textview125);

  frame300 = gtk_frame_new (NULL);
  gtk_widget_show (frame300);
  gtk_box_pack_start (GTK_BOX (hbox335), frame300, FALSE, FALSE, 0);

  alignment435 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment435);
  gtk_container_add (GTK_CONTAINER (frame300), alignment435);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment435), 0, 0, 12, 0);

  hbox336 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox336);
  gtk_container_add (GTK_CONTAINER (alignment435), hbox336);

  entry74 = gtk_entry_new ();
  gtk_widget_show (entry74);
  gtk_box_pack_start (GTK_BOX (hbox336), entry74, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry74, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry74), 8226);

  button169 = gtk_button_new ();
  gtk_widget_show (button169);
  gtk_box_pack_start (GTK_BOX (hbox336), button169, FALSE, FALSE, 0);

  image207 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image207);
  gtk_container_add (GTK_CONTAINER (button169), image207);

  label11295 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11295);
  gtk_frame_set_label_widget (GTK_FRAME (frame300), label11295);
  gtk_label_set_use_markup (GTK_LABEL (label11295), TRUE);

  label11296 = gtk_label_new (_("Extern It"));
  gtk_widget_show (label11296);
  gtk_frame_set_label_widget (GTK_FRAME (frame299), label11296);
  gtk_label_set_use_markup (GTK_LABEL (label11296), TRUE);

  frame301 = gtk_frame_new (NULL);
  gtk_widget_show (frame301);
  gtk_table_attach (GTK_TABLE (table36), frame301, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame301, 83, 9);

  alignment436 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment436);
  gtk_container_add (GTK_CONTAINER (frame301), alignment436);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment436), 0, 0, 12, 0);

  alignment437 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment437);
  gtk_container_add (GTK_CONTAINER (alignment436), alignment437);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment437), 0, 0, 12, 0);

  hbox337 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox337);
  gtk_container_add (GTK_CONTAINER (alignment437), hbox337);

  label11297 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11297);
  gtk_box_pack_start (GTK_BOX (hbox337), label11297, FALSE, FALSE, 0);

  scrolledwindow167 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow167);
  gtk_box_pack_start (GTK_BOX (hbox337), scrolledwindow167, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow167), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow167), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow167), GTK_SHADOW_IN);

  textview126 = gtk_text_view_new ();
  gtk_widget_show (textview126);
  gtk_container_add (GTK_CONTAINER (scrolledwindow167), textview126);
  gtk_widget_set_size_request (textview126, -1, 8);

  label11298 = gtk_label_new (_("Servo"));
  gtk_widget_show (label11298);
  gtk_frame_set_label_widget (GTK_FRAME (frame301), label11298);
  gtk_label_set_use_markup (GTK_LABEL (label11298), TRUE);

  frame302 = gtk_frame_new (NULL);
  gtk_widget_show (frame302);
  gtk_table_attach (GTK_TABLE (table36), frame302, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment438 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment438);
  gtk_container_add (GTK_CONTAINER (frame302), alignment438);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment438), 8, 8, 5, 0);

  hbox338 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox338);
  gtk_container_add (GTK_CONTAINER (alignment438), hbox338);

  label11299 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11299);
  gtk_box_pack_start (GTK_BOX (hbox338), label11299, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label11299, 46, -1);

  scrolledwindow168 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow168);
  gtk_box_pack_start (GTK_BOX (hbox338), scrolledwindow168, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow168), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow168), GTK_SHADOW_IN);

  textview127 = gtk_text_view_new ();
  gtk_widget_show (textview127);
  gtk_container_add (GTK_CONTAINER (scrolledwindow168), textview127);

  frame303 = gtk_frame_new (NULL);
  gtk_widget_show (frame303);
  gtk_box_pack_start (GTK_BOX (hbox338), frame303, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame303), 0.01, 0.5);

  alignment439 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment439);
  gtk_container_add (GTK_CONTAINER (frame303), alignment439);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment439), 0, 0, 12, 0);

  hbox339 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox339);
  gtk_container_add (GTK_CONTAINER (alignment439), hbox339);

  entry75 = gtk_entry_new ();
  gtk_widget_show (entry75);
  gtk_box_pack_start (GTK_BOX (hbox339), entry75, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry75, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry75), 8226);

  button170 = gtk_button_new ();
  gtk_widget_show (button170);
  gtk_box_pack_start (GTK_BOX (hbox339), button170, FALSE, FALSE, 0);

  image208 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image208);
  gtk_container_add (GTK_CONTAINER (button170), image208);

  label11300 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11300);
  gtk_frame_set_label_widget (GTK_FRAME (frame303), label11300);
  gtk_widget_set_size_request (label11300, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11300), TRUE);

  label11301 = gtk_label_new (_("QEI"));
  gtk_widget_show (label11301);
  gtk_frame_set_label_widget (GTK_FRAME (frame302), label11301);
  gtk_label_set_use_markup (GTK_LABEL (label11301), TRUE);

  frame304 = gtk_frame_new (NULL);
  gtk_widget_show (frame304);
  gtk_table_attach (GTK_TABLE (table36), frame304, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment440 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment440);
  gtk_container_add (GTK_CONTAINER (frame304), alignment440);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment440), 0, 0, 12, 0);

  alignment441 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment441);
  gtk_container_add (GTK_CONTAINER (alignment440), alignment441);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment441), 8, 8, 5, 0);

  hbox340 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox340);
  gtk_container_add (GTK_CONTAINER (alignment441), hbox340);

  label11302 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11302);
  gtk_box_pack_start (GTK_BOX (hbox340), label11302, FALSE, FALSE, 0);

  scrolledwindow169 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow169);
  gtk_box_pack_start (GTK_BOX (hbox340), scrolledwindow169, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow169), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow169), GTK_SHADOW_IN);

  textview128 = gtk_text_view_new ();
  gtk_widget_show (textview128);
  gtk_container_add (GTK_CONTAINER (scrolledwindow169), textview128);

  frame305 = gtk_frame_new (NULL);
  gtk_widget_show (frame305);
  gtk_box_pack_start (GTK_BOX (hbox340), frame305, FALSE, FALSE, 0);

  alignment442 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment442);
  gtk_container_add (GTK_CONTAINER (frame305), alignment442);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment442), 0, 0, 12, 0);

  hbox341 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox341);
  gtk_container_add (GTK_CONTAINER (alignment442), hbox341);

  entry76 = gtk_entry_new ();
  gtk_widget_show (entry76);
  gtk_box_pack_start (GTK_BOX (hbox341), entry76, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry76, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry76), 8226);

  button171 = gtk_button_new ();
  gtk_widget_show (button171);
  gtk_box_pack_start (GTK_BOX (hbox341), button171, FALSE, FALSE, 0);

  image209 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image209);
  gtk_container_add (GTK_CONTAINER (button171), image209);

  label11303 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11303);
  gtk_frame_set_label_widget (GTK_FRAME (frame305), label11303);
  gtk_label_set_use_markup (GTK_LABEL (label11303), TRUE);

  label11304 = gtk_label_new (_("DCM"));
  gtk_widget_show (label11304);
  gtk_frame_set_label_widget (GTK_FRAME (frame304), label11304);
  gtk_label_set_use_markup (GTK_LABEL (label11304), TRUE);

  frame306 = gtk_frame_new (NULL);
  gtk_widget_show (frame306);
  gtk_table_attach (GTK_TABLE (table36), frame306, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame306), GTK_SHADOW_NONE);

  alignment443 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment443);
  gtk_container_add (GTK_CONTAINER (frame306), alignment443);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment443), 0, 0, 12, 0);

  drawingarea12 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea12);
  gtk_container_add (GTK_CONTAINER (alignment443), drawingarea12);

  label11305 = gtk_label_new ("");
  gtk_widget_show (label11305);
  gtk_frame_set_label_widget (GTK_FRAME (frame306), label11305);
  gtk_label_set_use_markup (GTK_LABEL (label11305), TRUE);

  frame307 = gtk_frame_new (NULL);
  gtk_widget_show (frame307);
  gtk_table_attach (GTK_TABLE (table36), frame307, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment444 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment444);
  gtk_container_add (GTK_CONTAINER (frame307), alignment444);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment444), 0, 0, 12, 0);

  alignment445 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment445);
  gtk_container_add (GTK_CONTAINER (alignment444), alignment445);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment445), 8, 8, 5, 0);

  hbox342 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox342);
  gtk_container_add (GTK_CONTAINER (alignment445), hbox342);

  label11306 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11306);
  gtk_box_pack_start (GTK_BOX (hbox342), label11306, FALSE, FALSE, 0);

  scrolledwindow170 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow170);
  gtk_box_pack_start (GTK_BOX (hbox342), scrolledwindow170, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow170), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow170), GTK_SHADOW_IN);

  textview129 = gtk_text_view_new ();
  gtk_widget_show (textview129);
  gtk_container_add (GTK_CONTAINER (scrolledwindow170), textview129);

  frame308 = gtk_frame_new (NULL);
  gtk_widget_show (frame308);
  gtk_box_pack_start (GTK_BOX (hbox342), frame308, FALSE, FALSE, 0);

  alignment446 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment446);
  gtk_container_add (GTK_CONTAINER (frame308), alignment446);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment446), 0, 0, 12, 0);

  hbox343 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox343);
  gtk_container_add (GTK_CONTAINER (alignment446), hbox343);

  entry77 = gtk_entry_new ();
  gtk_widget_show (entry77);
  gtk_box_pack_start (GTK_BOX (hbox343), entry77, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry77, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry77), 8226);

  button172 = gtk_button_new ();
  gtk_widget_show (button172);
  gtk_box_pack_start (GTK_BOX (hbox343), button172, FALSE, FALSE, 0);

  image210 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image210);
  gtk_container_add (GTK_CONTAINER (button172), image210);

  label11307 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11307);
  gtk_frame_set_label_widget (GTK_FRAME (frame308), label11307);
  gtk_label_set_use_markup (GTK_LABEL (label11307), TRUE);

  label11308 = gtk_label_new (_("AX12"));
  gtk_widget_show (label11308);
  gtk_frame_set_label_widget (GTK_FRAME (frame307), label11308);
  gtk_label_set_use_markup (GTK_LABEL (label11308), TRUE);

  frame309 = gtk_frame_new (NULL);
  gtk_widget_show (frame309);
  gtk_table_attach (GTK_TABLE (table36), frame309, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment447 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment447);
  gtk_container_add (GTK_CONTAINER (frame309), alignment447);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment447), 0, 0, 12, 0);

  hbox344 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox344);
  gtk_container_add (GTK_CONTAINER (alignment447), hbox344);
  gtk_container_set_border_width (GTK_CONTAINER (hbox344), 5);

  frame310 = gtk_frame_new (NULL);
  gtk_widget_show (frame310);
  gtk_box_pack_start (GTK_BOX (hbox344), frame310, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame310, 17, -1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame310), GTK_SHADOW_NONE);

  alignment448 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment448);
  gtk_container_add (GTK_CONTAINER (frame310), alignment448);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment448), 0, 0, 12, 0);

  scrolledwindow171 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow171);
  gtk_container_add (GTK_CONTAINER (alignment448), scrolledwindow171);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow171), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow171), GTK_SHADOW_IN);

  textview130 = gtk_text_view_new ();
  gtk_widget_show (textview130);
  gtk_container_add (GTK_CONTAINER (scrolledwindow171), textview130);

  label11309 = gtk_label_new (_("N\302\260 ADC"));
  gtk_widget_show (label11309);
  gtk_frame_set_label_widget (GTK_FRAME (frame310), label11309);
  gtk_label_set_use_markup (GTK_LABEL (label11309), TRUE);

  frame311 = gtk_frame_new (NULL);
  gtk_widget_show (frame311);
  gtk_box_pack_start (GTK_BOX (hbox344), frame311, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame311), 0.01, 0.5);

  alignment449 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment449);
  gtk_container_add (GTK_CONTAINER (frame311), alignment449);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment449), 0, 0, 12, 0);

  hbox345 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox345);
  gtk_container_add (GTK_CONTAINER (alignment449), hbox345);

  entry78 = gtk_entry_new ();
  gtk_widget_show (entry78);
  gtk_box_pack_start (GTK_BOX (hbox345), entry78, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry78, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry78), 8226);

  button173 = gtk_button_new ();
  gtk_widget_show (button173);
  gtk_box_pack_start (GTK_BOX (hbox345), button173, FALSE, TRUE, 0);

  image211 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image211);
  gtk_container_add (GTK_CONTAINER (button173), image211);

  label11310 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11310);
  gtk_frame_set_label_widget (GTK_FRAME (frame311), label11310);
  gtk_widget_set_size_request (label11310, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11310), TRUE);

  label11311 = gtk_label_new (_("ADC"));
  gtk_widget_show (label11311);
  gtk_frame_set_label_widget (GTK_FRAME (frame309), label11311);
  gtk_label_set_use_markup (GTK_LABEL (label11311), TRUE);

  frame312 = gtk_frame_new (NULL);
  gtk_widget_show (frame312);
  gtk_table_attach (GTK_TABLE (table36), frame312, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment450 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment450);
  gtk_container_add (GTK_CONTAINER (frame312), alignment450);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment450), 0, 0, 12, 0);

  alignment451 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment451);
  gtk_container_add (GTK_CONTAINER (alignment450), alignment451);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment451), 0, 0, 12, 0);

  hbox346 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox346);
  gtk_container_add (GTK_CONTAINER (alignment451), hbox346);

  label11312 = gtk_label_new (_("Valeur       "));
  gtk_widget_show (label11312);
  gtk_box_pack_start (GTK_BOX (hbox346), label11312, FALSE, FALSE, 0);

  scrolledwindow172 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow172);
  gtk_box_pack_start (GTK_BOX (hbox346), scrolledwindow172, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow172), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow172), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow172), GTK_SHADOW_IN);

  textview131 = gtk_text_view_new ();
  gtk_widget_show (textview131);
  gtk_container_add (GTK_CONTAINER (scrolledwindow172), textview131);
  gtk_widget_set_size_request (textview131, -1, 8);

  label11313 = gtk_label_new (_("PWM"));
  gtk_widget_show (label11313);
  gtk_frame_set_label_widget (GTK_FRAME (frame312), label11313);
  gtk_label_set_use_markup (GTK_LABEL (label11313), TRUE);

  label666 = gtk_label_new (_("Strat\303\251gie"));
  gtk_widget_show (label666);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (cei_ong_all), gtk_notebook_get_nth_page (GTK_NOTEBOOK (cei_ong_all), 2), label666);

  vbox120 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox120);
  gtk_container_add (GTK_CONTAINER (cei_ong_all), vbox120);

  table37 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table37);
  gtk_box_pack_start (GTK_BOX (vbox120), table37, TRUE, TRUE, 0);

  scrolledwindow173 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow173);
  gtk_table_attach (GTK_TABLE (table37), scrolledwindow173, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow173), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow173), GTK_SHADOW_IN);

  viewport10 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport10);
  gtk_container_add (GTK_CONTAINER (scrolledwindow173), viewport10);
  gtk_widget_set_size_request (viewport10, 42, 196);

  vbox121 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox121);
  gtk_container_add (GTK_CONTAINER (viewport10), vbox121);

  hbox347 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox347);
  gtk_box_pack_start (GTK_BOX (vbox121), hbox347, TRUE, TRUE, 0);

  hbox348 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox348);
  gtk_box_pack_start (GTK_BOX (hbox347), hbox348, TRUE, TRUE, 0);

  label11314 = gtk_label_new (_("Error"));
  gtk_widget_show (label11314);
  gtk_box_pack_start (GTK_BOX (hbox348), label11314, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11314), GTK_JUSTIFY_CENTER);

  image212 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image212);
  gtk_box_pack_end (GTK_BOX (hbox348), image212, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image212, FALSE);

  hbox349 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox349);
  gtk_box_pack_start (GTK_BOX (vbox121), hbox349, TRUE, TRUE, 0);

  label11315 = gtk_label_new (_("User"));
  gtk_widget_show (label11315);
  gtk_box_pack_start (GTK_BOX (hbox349), label11315, TRUE, FALSE, 0);

  image213 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image213);
  gtk_box_pack_end (GTK_BOX (hbox349), image213, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image213, FALSE);

  hbox350 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox350);
  gtk_box_pack_start (GTK_BOX (vbox121), hbox350, FALSE, FALSE, 0);

  label11316 = gtk_label_new (_("User"));
  gtk_widget_show (label11316);
  gtk_box_pack_start (GTK_BOX (hbox350), label11316, TRUE, FALSE, 0);

  image214 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image214);
  gtk_box_pack_end (GTK_BOX (hbox350), image214, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image214, FALSE);

  hbox351 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox351);
  gtk_box_pack_start (GTK_BOX (vbox121), hbox351, TRUE, TRUE, 0);

  label11317 = gtk_label_new (_("Uart"));
  gtk_widget_show (label11317);
  gtk_box_pack_start (GTK_BOX (hbox351), label11317, TRUE, FALSE, 0);

  image215 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image215);
  gtk_box_pack_end (GTK_BOX (hbox351), image215, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image215, FALSE);

  hbox352 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox352);
  gtk_box_pack_start (GTK_BOX (vbox121), hbox352, TRUE, TRUE, 0);

  label11318 = gtk_label_new (_("Can"));
  gtk_widget_show (label11318);
  gtk_box_pack_start (GTK_BOX (hbox352), label11318, TRUE, FALSE, 0);

  image216 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image216);
  gtk_box_pack_end (GTK_BOX (hbox352), image216, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image216, FALSE);

  hbox353 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox353);
  gtk_box_pack_start (GTK_BOX (vbox121), hbox353, TRUE, TRUE, 0);

  label11319 = gtk_label_new (_("Run"));
  gtk_widget_show (label11319);
  gtk_box_pack_start (GTK_BOX (hbox353), label11319, TRUE, FALSE, 0);

  image217 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image217);
  gtk_box_pack_end (GTK_BOX (hbox353), image217, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image217, FALSE);

  frame313 = gtk_frame_new (NULL);
  gtk_widget_show (frame313);
  gtk_table_attach (GTK_TABLE (table37), frame313, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame313, 54, 43);

  alignment452 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment452);
  gtk_container_add (GTK_CONTAINER (frame313), alignment452);
  gtk_widget_set_size_request (alignment452, -1, 162);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment452), 0, 0, 12, 0);

  image218 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image218);
  gtk_container_add (GTK_CONTAINER (alignment452), image218);
  gtk_widget_set_size_request (image218, -1, 330);
  gtk_widget_set_sensitive (image218, FALSE);

  label11320 = gtk_label_new (_("Carte en\n fonctionnement"));
  gtk_widget_show (label11320);
  gtk_frame_set_label_widget (GTK_FRAME (frame313), label11320);
  gtk_label_set_use_markup (GTK_LABEL (label11320), TRUE);
  gtk_label_set_justify (GTK_LABEL (label11320), GTK_JUSTIFY_CENTER);

  hbuttonbox8 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox8);
  gtk_table_attach (GTK_TABLE (table37), hbuttonbox8, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hbuttonbox8, 449, 2);

  cei_sup_button_bt1 = gtk_button_new_with_mnemonic (_("BT1"));
  gtk_widget_show (cei_sup_button_bt1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), cei_sup_button_bt1);
  gtk_container_set_border_width (GTK_CONTAINER (cei_sup_button_bt1), 11);
  GTK_WIDGET_SET_FLAGS (cei_sup_button_bt1, GTK_CAN_DEFAULT);

  cei_sup_button_bt2 = gtk_button_new_with_mnemonic (_("BT2"));
  gtk_widget_show (cei_sup_button_bt2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), cei_sup_button_bt2);
  gtk_container_set_border_width (GTK_CONTAINER (cei_sup_button_bt2), 11);
  GTK_WIDGET_SET_FLAGS (cei_sup_button_bt2, GTK_CAN_DEFAULT);

  button176 = gtk_button_new_with_mnemonic (_("BT3"));
  gtk_widget_show (button176);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), button176);
  gtk_container_set_border_width (GTK_CONTAINER (button176), 11);
  GTK_WIDGET_SET_FLAGS (button176, GTK_CAN_DEFAULT);

  button177 = gtk_button_new_with_mnemonic (_("BT4"));
  gtk_widget_show (button177);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), button177);
  gtk_container_set_border_width (GTK_CONTAINER (button177), 11);
  GTK_WIDGET_SET_FLAGS (button177, GTK_CAN_DEFAULT);

  button178 = gtk_button_new_with_mnemonic (_("Reset"));
  gtk_widget_show (button178);
  gtk_container_add (GTK_CONTAINER (hbuttonbox8), button178);
  gtk_container_set_border_width (GTK_CONTAINER (button178), 11);
  GTK_WIDGET_SET_FLAGS (button178, GTK_CAN_DEFAULT);

  image219 = create_pixmap (Fenetre, "carte.png");
  gtk_widget_show (image219);
  gtk_table_attach (GTK_TABLE (table37), image219, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);

  vbox122 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox122);
  gtk_box_pack_start (GTK_BOX (vbox120), vbox122, TRUE, TRUE, 0);

  table38 = gtk_table_new (3, 3, TRUE);
  gtk_widget_show (table38);
  gtk_box_pack_start (GTK_BOX (vbox122), table38, TRUE, TRUE, 0);
  gtk_widget_set_size_request (table38, 477, 249);

  vbox123 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox123);
  gtk_table_attach (GTK_TABLE (table38), vbox123, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame314 = gtk_frame_new (NULL);
  gtk_widget_show (frame314);
  gtk_box_pack_start (GTK_BOX (vbox123), frame314, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame314, 41, 15);

  alignment453 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment453);
  gtk_container_add (GTK_CONTAINER (frame314), alignment453);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment453), 0, 0, 12, 0);

  hbox354 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox354);
  gtk_container_add (GTK_CONTAINER (alignment453), hbox354);

  label11321 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11321);
  gtk_box_pack_start (GTK_BOX (hbox354), label11321, FALSE, FALSE, 0);

  scrolledwindow174 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow174);
  gtk_box_pack_start (GTK_BOX (hbox354), scrolledwindow174, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow174), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow174), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow174), GTK_SHADOW_IN);

  textview132 = gtk_text_view_new ();
  gtk_widget_show (textview132);
  gtk_container_add (GTK_CONTAINER (scrolledwindow174), textview132);
  gtk_widget_set_size_request (textview132, -1, 8);

  label11322 = gtk_label_new (_("Step Motor"));
  gtk_widget_show (label11322);
  gtk_frame_set_label_widget (GTK_FRAME (frame314), label11322);
  gtk_label_set_use_markup (GTK_LABEL (label11322), TRUE);

  vbox124 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox124);
  gtk_table_attach (GTK_TABLE (table38), vbox124, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame315 = gtk_frame_new (NULL);
  gtk_widget_show (frame315);
  gtk_box_pack_start (GTK_BOX (vbox124), frame315, TRUE, TRUE, 0);

  alignment454 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment454);
  gtk_container_add (GTK_CONTAINER (frame315), alignment454);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment454), 0, 0, 12, 0);

  alignment455 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment455);
  gtk_container_add (GTK_CONTAINER (alignment454), alignment455);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment455), 8, 8, 5, 0);

  hbox355 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox355);
  gtk_container_add (GTK_CONTAINER (alignment455), hbox355);

  label11323 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11323);
  gtk_box_pack_start (GTK_BOX (hbox355), label11323, FALSE, FALSE, 0);

  scrolledwindow175 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow175);
  gtk_box_pack_start (GTK_BOX (hbox355), scrolledwindow175, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow175), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow175), GTK_SHADOW_IN);

  textview133 = gtk_text_view_new ();
  gtk_widget_show (textview133);
  gtk_container_add (GTK_CONTAINER (scrolledwindow175), textview133);

  frame316 = gtk_frame_new (NULL);
  gtk_widget_show (frame316);
  gtk_box_pack_start (GTK_BOX (hbox355), frame316, FALSE, FALSE, 0);

  alignment456 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment456);
  gtk_container_add (GTK_CONTAINER (frame316), alignment456);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment456), 0, 0, 12, 0);

  hbox356 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox356);
  gtk_container_add (GTK_CONTAINER (alignment456), hbox356);

  entry79 = gtk_entry_new ();
  gtk_widget_show (entry79);
  gtk_box_pack_start (GTK_BOX (hbox356), entry79, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry79, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry79), 8226);

  button179 = gtk_button_new ();
  gtk_widget_show (button179);
  gtk_box_pack_start (GTK_BOX (hbox356), button179, FALSE, FALSE, 0);

  image220 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image220);
  gtk_container_add (GTK_CONTAINER (button179), image220);

  label11324 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11324);
  gtk_frame_set_label_widget (GTK_FRAME (frame316), label11324);
  gtk_label_set_use_markup (GTK_LABEL (label11324), TRUE);

  label11325 = gtk_label_new (_("Extern It"));
  gtk_widget_show (label11325);
  gtk_frame_set_label_widget (GTK_FRAME (frame315), label11325);
  gtk_label_set_use_markup (GTK_LABEL (label11325), TRUE);

  frame317 = gtk_frame_new (NULL);
  gtk_widget_show (frame317);
  gtk_table_attach (GTK_TABLE (table38), frame317, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame317, 83, 9);

  alignment457 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment457);
  gtk_container_add (GTK_CONTAINER (frame317), alignment457);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment457), 0, 0, 12, 0);

  alignment458 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment458);
  gtk_container_add (GTK_CONTAINER (alignment457), alignment458);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment458), 0, 0, 12, 0);

  hbox357 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox357);
  gtk_container_add (GTK_CONTAINER (alignment458), hbox357);

  label11326 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11326);
  gtk_box_pack_start (GTK_BOX (hbox357), label11326, FALSE, FALSE, 0);

  scrolledwindow176 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow176);
  gtk_box_pack_start (GTK_BOX (hbox357), scrolledwindow176, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow176), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow176), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow176), GTK_SHADOW_IN);

  textview134 = gtk_text_view_new ();
  gtk_widget_show (textview134);
  gtk_container_add (GTK_CONTAINER (scrolledwindow176), textview134);
  gtk_widget_set_size_request (textview134, -1, 8);

  label11327 = gtk_label_new (_("Servo"));
  gtk_widget_show (label11327);
  gtk_frame_set_label_widget (GTK_FRAME (frame317), label11327);
  gtk_label_set_use_markup (GTK_LABEL (label11327), TRUE);

  frame318 = gtk_frame_new (NULL);
  gtk_widget_show (frame318);
  gtk_table_attach (GTK_TABLE (table38), frame318, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment459 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment459);
  gtk_container_add (GTK_CONTAINER (frame318), alignment459);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment459), 8, 8, 5, 0);

  hbox358 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox358);
  gtk_container_add (GTK_CONTAINER (alignment459), hbox358);

  label11328 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11328);
  gtk_box_pack_start (GTK_BOX (hbox358), label11328, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label11328, 46, -1);

  scrolledwindow177 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow177);
  gtk_box_pack_start (GTK_BOX (hbox358), scrolledwindow177, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow177), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow177), GTK_SHADOW_IN);

  textview135 = gtk_text_view_new ();
  gtk_widget_show (textview135);
  gtk_container_add (GTK_CONTAINER (scrolledwindow177), textview135);

  frame319 = gtk_frame_new (NULL);
  gtk_widget_show (frame319);
  gtk_box_pack_start (GTK_BOX (hbox358), frame319, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame319), 0.01, 0.5);

  alignment460 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment460);
  gtk_container_add (GTK_CONTAINER (frame319), alignment460);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment460), 0, 0, 12, 0);

  hbox359 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox359);
  gtk_container_add (GTK_CONTAINER (alignment460), hbox359);

  entry80 = gtk_entry_new ();
  gtk_widget_show (entry80);
  gtk_box_pack_start (GTK_BOX (hbox359), entry80, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry80, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry80), 8226);

  button180 = gtk_button_new ();
  gtk_widget_show (button180);
  gtk_box_pack_start (GTK_BOX (hbox359), button180, FALSE, FALSE, 0);

  image221 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image221);
  gtk_container_add (GTK_CONTAINER (button180), image221);

  label11329 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11329);
  gtk_frame_set_label_widget (GTK_FRAME (frame319), label11329);
  gtk_widget_set_size_request (label11329, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11329), TRUE);

  label11330 = gtk_label_new (_("QEI"));
  gtk_widget_show (label11330);
  gtk_frame_set_label_widget (GTK_FRAME (frame318), label11330);
  gtk_label_set_use_markup (GTK_LABEL (label11330), TRUE);

  frame320 = gtk_frame_new (NULL);
  gtk_widget_show (frame320);
  gtk_table_attach (GTK_TABLE (table38), frame320, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment461 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment461);
  gtk_container_add (GTK_CONTAINER (frame320), alignment461);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment461), 0, 0, 12, 0);

  alignment462 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment462);
  gtk_container_add (GTK_CONTAINER (alignment461), alignment462);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment462), 8, 8, 5, 0);

  hbox360 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox360);
  gtk_container_add (GTK_CONTAINER (alignment462), hbox360);

  label11331 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11331);
  gtk_box_pack_start (GTK_BOX (hbox360), label11331, FALSE, FALSE, 0);

  scrolledwindow178 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow178);
  gtk_box_pack_start (GTK_BOX (hbox360), scrolledwindow178, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow178), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow178), GTK_SHADOW_IN);

  textview136 = gtk_text_view_new ();
  gtk_widget_show (textview136);
  gtk_container_add (GTK_CONTAINER (scrolledwindow178), textview136);

  frame321 = gtk_frame_new (NULL);
  gtk_widget_show (frame321);
  gtk_box_pack_start (GTK_BOX (hbox360), frame321, FALSE, FALSE, 0);

  alignment463 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment463);
  gtk_container_add (GTK_CONTAINER (frame321), alignment463);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment463), 0, 0, 12, 0);

  hbox361 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox361);
  gtk_container_add (GTK_CONTAINER (alignment463), hbox361);

  entry81 = gtk_entry_new ();
  gtk_widget_show (entry81);
  gtk_box_pack_start (GTK_BOX (hbox361), entry81, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry81, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry81), 8226);

  button181 = gtk_button_new ();
  gtk_widget_show (button181);
  gtk_box_pack_start (GTK_BOX (hbox361), button181, FALSE, FALSE, 0);

  image222 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image222);
  gtk_container_add (GTK_CONTAINER (button181), image222);

  label11332 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11332);
  gtk_frame_set_label_widget (GTK_FRAME (frame321), label11332);
  gtk_label_set_use_markup (GTK_LABEL (label11332), TRUE);

  label11333 = gtk_label_new (_("DCM"));
  gtk_widget_show (label11333);
  gtk_frame_set_label_widget (GTK_FRAME (frame320), label11333);
  gtk_label_set_use_markup (GTK_LABEL (label11333), TRUE);

  frame322 = gtk_frame_new (NULL);
  gtk_widget_show (frame322);
  gtk_table_attach (GTK_TABLE (table38), frame322, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame322), GTK_SHADOW_NONE);

  alignment464 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment464);
  gtk_container_add (GTK_CONTAINER (frame322), alignment464);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment464), 0, 0, 12, 0);

  drawingarea13 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea13);
  gtk_container_add (GTK_CONTAINER (alignment464), drawingarea13);

  label11334 = gtk_label_new ("");
  gtk_widget_show (label11334);
  gtk_frame_set_label_widget (GTK_FRAME (frame322), label11334);
  gtk_label_set_use_markup (GTK_LABEL (label11334), TRUE);

  frame323 = gtk_frame_new (NULL);
  gtk_widget_show (frame323);
  gtk_table_attach (GTK_TABLE (table38), frame323, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment465 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment465);
  gtk_container_add (GTK_CONTAINER (frame323), alignment465);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment465), 0, 0, 12, 0);

  alignment466 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment466);
  gtk_container_add (GTK_CONTAINER (alignment465), alignment466);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment466), 8, 8, 5, 0);

  hbox362 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox362);
  gtk_container_add (GTK_CONTAINER (alignment466), hbox362);

  label11335 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11335);
  gtk_box_pack_start (GTK_BOX (hbox362), label11335, FALSE, FALSE, 0);

  scrolledwindow179 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow179);
  gtk_box_pack_start (GTK_BOX (hbox362), scrolledwindow179, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow179), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow179), GTK_SHADOW_IN);

  textview137 = gtk_text_view_new ();
  gtk_widget_show (textview137);
  gtk_container_add (GTK_CONTAINER (scrolledwindow179), textview137);

  frame324 = gtk_frame_new (NULL);
  gtk_widget_show (frame324);
  gtk_box_pack_start (GTK_BOX (hbox362), frame324, FALSE, FALSE, 0);

  alignment467 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment467);
  gtk_container_add (GTK_CONTAINER (frame324), alignment467);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment467), 0, 0, 12, 0);

  hbox363 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox363);
  gtk_container_add (GTK_CONTAINER (alignment467), hbox363);

  entry82 = gtk_entry_new ();
  gtk_widget_show (entry82);
  gtk_box_pack_start (GTK_BOX (hbox363), entry82, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry82, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry82), 8226);

  button182 = gtk_button_new ();
  gtk_widget_show (button182);
  gtk_box_pack_start (GTK_BOX (hbox363), button182, FALSE, FALSE, 0);

  image223 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image223);
  gtk_container_add (GTK_CONTAINER (button182), image223);

  label11336 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11336);
  gtk_frame_set_label_widget (GTK_FRAME (frame324), label11336);
  gtk_label_set_use_markup (GTK_LABEL (label11336), TRUE);

  label11337 = gtk_label_new (_("AX12"));
  gtk_widget_show (label11337);
  gtk_frame_set_label_widget (GTK_FRAME (frame323), label11337);
  gtk_label_set_use_markup (GTK_LABEL (label11337), TRUE);

  frame325 = gtk_frame_new (NULL);
  gtk_widget_show (frame325);
  gtk_table_attach (GTK_TABLE (table38), frame325, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment468 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment468);
  gtk_container_add (GTK_CONTAINER (frame325), alignment468);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment468), 0, 0, 12, 0);

  hbox364 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox364);
  gtk_container_add (GTK_CONTAINER (alignment468), hbox364);
  gtk_container_set_border_width (GTK_CONTAINER (hbox364), 5);

  frame326 = gtk_frame_new (NULL);
  gtk_widget_show (frame326);
  gtk_box_pack_start (GTK_BOX (hbox364), frame326, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame326, 17, -1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame326), GTK_SHADOW_NONE);

  alignment469 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment469);
  gtk_container_add (GTK_CONTAINER (frame326), alignment469);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment469), 0, 0, 12, 0);

  scrolledwindow180 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow180);
  gtk_container_add (GTK_CONTAINER (alignment469), scrolledwindow180);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow180), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow180), GTK_SHADOW_IN);

  textview138 = gtk_text_view_new ();
  gtk_widget_show (textview138);
  gtk_container_add (GTK_CONTAINER (scrolledwindow180), textview138);

  label11338 = gtk_label_new (_("N\302\260 ADC"));
  gtk_widget_show (label11338);
  gtk_frame_set_label_widget (GTK_FRAME (frame326), label11338);
  gtk_label_set_use_markup (GTK_LABEL (label11338), TRUE);

  frame327 = gtk_frame_new (NULL);
  gtk_widget_show (frame327);
  gtk_box_pack_start (GTK_BOX (hbox364), frame327, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame327), 0.01, 0.5);

  alignment470 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment470);
  gtk_container_add (GTK_CONTAINER (frame327), alignment470);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment470), 0, 0, 12, 0);

  hbox365 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox365);
  gtk_container_add (GTK_CONTAINER (alignment470), hbox365);

  entry83 = gtk_entry_new ();
  gtk_widget_show (entry83);
  gtk_box_pack_start (GTK_BOX (hbox365), entry83, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry83, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry83), 8226);

  button183 = gtk_button_new ();
  gtk_widget_show (button183);
  gtk_box_pack_start (GTK_BOX (hbox365), button183, FALSE, TRUE, 0);

  image224 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image224);
  gtk_container_add (GTK_CONTAINER (button183), image224);

  label11339 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11339);
  gtk_frame_set_label_widget (GTK_FRAME (frame327), label11339);
  gtk_widget_set_size_request (label11339, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11339), TRUE);

  label11340 = gtk_label_new (_("ADC"));
  gtk_widget_show (label11340);
  gtk_frame_set_label_widget (GTK_FRAME (frame325), label11340);
  gtk_label_set_use_markup (GTK_LABEL (label11340), TRUE);

  frame328 = gtk_frame_new (NULL);
  gtk_widget_show (frame328);
  gtk_table_attach (GTK_TABLE (table38), frame328, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment471 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment471);
  gtk_container_add (GTK_CONTAINER (frame328), alignment471);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment471), 0, 0, 12, 0);

  alignment472 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment472);
  gtk_container_add (GTK_CONTAINER (alignment471), alignment472);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment472), 0, 0, 12, 0);

  hbox366 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox366);
  gtk_container_add (GTK_CONTAINER (alignment472), hbox366);

  label11341 = gtk_label_new (_("Valeur       "));
  gtk_widget_show (label11341);
  gtk_box_pack_start (GTK_BOX (hbox366), label11341, FALSE, FALSE, 0);

  scrolledwindow181 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow181);
  gtk_box_pack_start (GTK_BOX (hbox366), scrolledwindow181, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow181), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow181), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow181), GTK_SHADOW_IN);

  textview139 = gtk_text_view_new ();
  gtk_widget_show (textview139);
  gtk_container_add (GTK_CONTAINER (scrolledwindow181), textview139);
  gtk_widget_set_size_request (textview139, -1, 8);

  label11342 = gtk_label_new (_("PWM"));
  gtk_widget_show (label11342);
  gtk_frame_set_label_widget (GTK_FRAME (frame328), label11342);
  gtk_label_set_use_markup (GTK_LABEL (label11342), TRUE);

  label20 = gtk_label_new (_("Supervision"));
  gtk_widget_show (label20);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (cei_ong_all), gtk_notebook_get_nth_page (GTK_NOTEBOOK (cei_ong_all), 3), label20);

  vbox125 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox125);
  gtk_container_add (GTK_CONTAINER (cei_ong_all), vbox125);

  table39 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table39);
  gtk_box_pack_start (GTK_BOX (vbox125), table39, TRUE, TRUE, 0);

  scrolledwindow182 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow182);
  gtk_table_attach (GTK_TABLE (table39), scrolledwindow182, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow182), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow182), GTK_SHADOW_IN);

  viewport11 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport11);
  gtk_container_add (GTK_CONTAINER (scrolledwindow182), viewport11);
  gtk_widget_set_size_request (viewport11, 42, 196);

  vbox126 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox126);
  gtk_container_add (GTK_CONTAINER (viewport11), vbox126);

  hbox367 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox367);
  gtk_box_pack_start (GTK_BOX (vbox126), hbox367, TRUE, TRUE, 0);

  hbox368 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox368);
  gtk_box_pack_start (GTK_BOX (hbox367), hbox368, TRUE, TRUE, 0);

  label11343 = gtk_label_new (_("Error"));
  gtk_widget_show (label11343);
  gtk_box_pack_start (GTK_BOX (hbox368), label11343, TRUE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label11343), GTK_JUSTIFY_CENTER);

  image225 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image225);
  gtk_box_pack_end (GTK_BOX (hbox368), image225, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image225, FALSE);

  hbox369 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox369);
  gtk_box_pack_start (GTK_BOX (vbox126), hbox369, TRUE, TRUE, 0);

  label11344 = gtk_label_new (_("User"));
  gtk_widget_show (label11344);
  gtk_box_pack_start (GTK_BOX (hbox369), label11344, TRUE, FALSE, 0);

  image226 = create_pixmap (Fenetre, "ledRouge.png");
  gtk_widget_show (image226);
  gtk_box_pack_end (GTK_BOX (hbox369), image226, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (image226, FALSE);

  hbox370 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox370);
  gtk_box_pack_start (GTK_BOX (vbox126), hbox370, FALSE, FALSE, 0);

  label11345 = gtk_label_new (_("User"));
  gtk_widget_show (label11345);
  gtk_box_pack_start (GTK_BOX (hbox370), label11345, TRUE, FALSE, 0);

  image227 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image227);
  gtk_box_pack_end (GTK_BOX (hbox370), image227, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image227, FALSE);

  hbox371 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox371);
  gtk_box_pack_start (GTK_BOX (vbox126), hbox371, TRUE, TRUE, 0);

  label11346 = gtk_label_new (_("Uart"));
  gtk_widget_show (label11346);
  gtk_box_pack_start (GTK_BOX (hbox371), label11346, TRUE, FALSE, 0);

  image228 = create_pixmap (Fenetre, "ledJaune.png");
  gtk_widget_show (image228);
  gtk_box_pack_end (GTK_BOX (hbox371), image228, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image228, FALSE);

  hbox372 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox372);
  gtk_box_pack_start (GTK_BOX (vbox126), hbox372, TRUE, TRUE, 0);

  label11347 = gtk_label_new (_("Can"));
  gtk_widget_show (label11347);
  gtk_box_pack_start (GTK_BOX (hbox372), label11347, TRUE, FALSE, 0);

  image229 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image229);
  gtk_box_pack_end (GTK_BOX (hbox372), image229, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image229, FALSE);

  hbox373 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox373);
  gtk_box_pack_start (GTK_BOX (vbox126), hbox373, TRUE, TRUE, 0);

  label11348 = gtk_label_new (_("Run"));
  gtk_widget_show (label11348);
  gtk_box_pack_start (GTK_BOX (hbox373), label11348, TRUE, FALSE, 0);

  image230 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image230);
  gtk_box_pack_end (GTK_BOX (hbox373), image230, FALSE, TRUE, 0);
  gtk_widget_set_sensitive (image230, FALSE);

  frame329 = gtk_frame_new (NULL);
  gtk_widget_show (frame329);
  gtk_table_attach (GTK_TABLE (table39), frame329, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame329, 54, 43);

  alignment473 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment473);
  gtk_container_add (GTK_CONTAINER (frame329), alignment473);
  gtk_widget_set_size_request (alignment473, -1, 162);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment473), 0, 0, 12, 0);

  image231 = create_pixmap (Fenetre, "ledVerte.png");
  gtk_widget_show (image231);
  gtk_container_add (GTK_CONTAINER (alignment473), image231);
  gtk_widget_set_size_request (image231, -1, 330);
  gtk_widget_set_sensitive (image231, FALSE);

  label11349 = gtk_label_new (_("Carte en\n fonctionnement"));
  gtk_widget_show (label11349);
  gtk_frame_set_label_widget (GTK_FRAME (frame329), label11349);
  gtk_label_set_use_markup (GTK_LABEL (label11349), TRUE);
  gtk_label_set_justify (GTK_LABEL (label11349), GTK_JUSTIFY_CENTER);

  hbuttonbox9 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox9);
  gtk_table_attach (GTK_TABLE (table39), hbuttonbox9, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_widget_set_size_request (hbuttonbox9, 449, 2);

  button184 = gtk_button_new_with_mnemonic (_("BT1"));
  gtk_widget_show (button184);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), button184);
  gtk_container_set_border_width (GTK_CONTAINER (button184), 11);
  GTK_WIDGET_SET_FLAGS (button184, GTK_CAN_DEFAULT);

  button185 = gtk_button_new_with_mnemonic (_("BT2"));
  gtk_widget_show (button185);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), button185);
  gtk_container_set_border_width (GTK_CONTAINER (button185), 11);
  GTK_WIDGET_SET_FLAGS (button185, GTK_CAN_DEFAULT);

  button186 = gtk_button_new_with_mnemonic (_("BT3"));
  gtk_widget_show (button186);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), button186);
  gtk_container_set_border_width (GTK_CONTAINER (button186), 11);
  GTK_WIDGET_SET_FLAGS (button186, GTK_CAN_DEFAULT);

  button187 = gtk_button_new_with_mnemonic (_("BT4"));
  gtk_widget_show (button187);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), button187);
  gtk_container_set_border_width (GTK_CONTAINER (button187), 11);
  GTK_WIDGET_SET_FLAGS (button187, GTK_CAN_DEFAULT);

  button188 = gtk_button_new_with_mnemonic (_("Reset"));
  gtk_widget_show (button188);
  gtk_container_add (GTK_CONTAINER (hbuttonbox9), button188);
  gtk_container_set_border_width (GTK_CONTAINER (button188), 11);
  GTK_WIDGET_SET_FLAGS (button188, GTK_CAN_DEFAULT);

  image232 = create_pixmap (Fenetre, "carte.png");
  gtk_widget_show (image232);
  gtk_table_attach (GTK_TABLE (table39), image232, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);

  vbox127 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox127);
  gtk_box_pack_start (GTK_BOX (vbox125), vbox127, TRUE, TRUE, 0);

  table40 = gtk_table_new (3, 3, TRUE);
  gtk_widget_show (table40);
  gtk_box_pack_start (GTK_BOX (vbox127), table40, TRUE, TRUE, 0);
  gtk_widget_set_size_request (table40, 477, 249);

  vbox128 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox128);
  gtk_table_attach (GTK_TABLE (table40), vbox128, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame330 = gtk_frame_new (NULL);
  gtk_widget_show (frame330);
  gtk_box_pack_start (GTK_BOX (vbox128), frame330, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame330, 41, 15);

  alignment474 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment474);
  gtk_container_add (GTK_CONTAINER (frame330), alignment474);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment474), 0, 0, 12, 0);

  hbox374 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox374);
  gtk_container_add (GTK_CONTAINER (alignment474), hbox374);

  label11350 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11350);
  gtk_box_pack_start (GTK_BOX (hbox374), label11350, FALSE, FALSE, 0);

  scrolledwindow183 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow183);
  gtk_box_pack_start (GTK_BOX (hbox374), scrolledwindow183, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow183), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow183), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow183), GTK_SHADOW_IN);

  textview140 = gtk_text_view_new ();
  gtk_widget_show (textview140);
  gtk_container_add (GTK_CONTAINER (scrolledwindow183), textview140);
  gtk_widget_set_size_request (textview140, -1, 8);

  label11351 = gtk_label_new (_("Step Motor"));
  gtk_widget_show (label11351);
  gtk_frame_set_label_widget (GTK_FRAME (frame330), label11351);
  gtk_label_set_use_markup (GTK_LABEL (label11351), TRUE);

  vbox129 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox129);
  gtk_table_attach (GTK_TABLE (table40), vbox129, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  frame331 = gtk_frame_new (NULL);
  gtk_widget_show (frame331);
  gtk_box_pack_start (GTK_BOX (vbox129), frame331, TRUE, TRUE, 0);

  alignment475 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment475);
  gtk_container_add (GTK_CONTAINER (frame331), alignment475);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment475), 0, 0, 12, 0);

  alignment476 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment476);
  gtk_container_add (GTK_CONTAINER (alignment475), alignment476);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment476), 8, 8, 5, 0);

  hbox375 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox375);
  gtk_container_add (GTK_CONTAINER (alignment476), hbox375);

  label11352 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11352);
  gtk_box_pack_start (GTK_BOX (hbox375), label11352, FALSE, FALSE, 0);

  scrolledwindow184 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow184);
  gtk_box_pack_start (GTK_BOX (hbox375), scrolledwindow184, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow184), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow184), GTK_SHADOW_IN);

  textview141 = gtk_text_view_new ();
  gtk_widget_show (textview141);
  gtk_container_add (GTK_CONTAINER (scrolledwindow184), textview141);

  frame332 = gtk_frame_new (NULL);
  gtk_widget_show (frame332);
  gtk_box_pack_start (GTK_BOX (hbox375), frame332, FALSE, FALSE, 0);

  alignment477 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment477);
  gtk_container_add (GTK_CONTAINER (frame332), alignment477);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment477), 0, 0, 12, 0);

  hbox376 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox376);
  gtk_container_add (GTK_CONTAINER (alignment477), hbox376);

  entry84 = gtk_entry_new ();
  gtk_widget_show (entry84);
  gtk_box_pack_start (GTK_BOX (hbox376), entry84, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry84, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry84), 8226);

  button189 = gtk_button_new ();
  gtk_widget_show (button189);
  gtk_box_pack_start (GTK_BOX (hbox376), button189, FALSE, FALSE, 0);

  image233 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image233);
  gtk_container_add (GTK_CONTAINER (button189), image233);

  label11353 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11353);
  gtk_frame_set_label_widget (GTK_FRAME (frame332), label11353);
  gtk_label_set_use_markup (GTK_LABEL (label11353), TRUE);

  label11354 = gtk_label_new (_("Extern It"));
  gtk_widget_show (label11354);
  gtk_frame_set_label_widget (GTK_FRAME (frame331), label11354);
  gtk_label_set_use_markup (GTK_LABEL (label11354), TRUE);

  frame333 = gtk_frame_new (NULL);
  gtk_widget_show (frame333);
  gtk_table_attach (GTK_TABLE (table40), frame333, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (frame333, 83, 9);

  alignment478 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment478);
  gtk_container_add (GTK_CONTAINER (frame333), alignment478);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment478), 0, 0, 12, 0);

  alignment479 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment479);
  gtk_container_add (GTK_CONTAINER (alignment478), alignment479);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment479), 0, 0, 12, 0);

  hbox377 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox377);
  gtk_container_add (GTK_CONTAINER (alignment479), hbox377);

  label11355 = gtk_label_new (_("Consigne "));
  gtk_widget_show (label11355);
  gtk_box_pack_start (GTK_BOX (hbox377), label11355, FALSE, FALSE, 0);

  scrolledwindow185 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow185);
  gtk_box_pack_start (GTK_BOX (hbox377), scrolledwindow185, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow185), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow185), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow185), GTK_SHADOW_IN);

  textview142 = gtk_text_view_new ();
  gtk_widget_show (textview142);
  gtk_container_add (GTK_CONTAINER (scrolledwindow185), textview142);
  gtk_widget_set_size_request (textview142, -1, 8);

  label11356 = gtk_label_new (_("Servo"));
  gtk_widget_show (label11356);
  gtk_frame_set_label_widget (GTK_FRAME (frame333), label11356);
  gtk_label_set_use_markup (GTK_LABEL (label11356), TRUE);

  frame334 = gtk_frame_new (NULL);
  gtk_widget_show (frame334);
  gtk_table_attach (GTK_TABLE (table40), frame334, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment480 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment480);
  gtk_container_add (GTK_CONTAINER (frame334), alignment480);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment480), 8, 8, 5, 0);

  hbox378 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox378);
  gtk_container_add (GTK_CONTAINER (alignment480), hbox378);

  label11357 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11357);
  gtk_box_pack_start (GTK_BOX (hbox378), label11357, FALSE, FALSE, 0);
  gtk_widget_set_size_request (label11357, 46, -1);

  scrolledwindow186 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow186);
  gtk_box_pack_start (GTK_BOX (hbox378), scrolledwindow186, TRUE, TRUE, 7);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow186), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow186), GTK_SHADOW_IN);

  textview143 = gtk_text_view_new ();
  gtk_widget_show (textview143);
  gtk_container_add (GTK_CONTAINER (scrolledwindow186), textview143);

  frame335 = gtk_frame_new (NULL);
  gtk_widget_show (frame335);
  gtk_box_pack_start (GTK_BOX (hbox378), frame335, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame335), 0.01, 0.5);

  alignment481 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment481);
  gtk_container_add (GTK_CONTAINER (frame335), alignment481);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment481), 0, 0, 12, 0);

  hbox379 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox379);
  gtk_container_add (GTK_CONTAINER (alignment481), hbox379);

  entry85 = gtk_entry_new ();
  gtk_widget_show (entry85);
  gtk_box_pack_start (GTK_BOX (hbox379), entry85, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry85, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry85), 8226);

  button190 = gtk_button_new ();
  gtk_widget_show (button190);
  gtk_box_pack_start (GTK_BOX (hbox379), button190, FALSE, FALSE, 0);

  image234 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image234);
  gtk_container_add (GTK_CONTAINER (button190), image234);

  label11358 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11358);
  gtk_frame_set_label_widget (GTK_FRAME (frame335), label11358);
  gtk_widget_set_size_request (label11358, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11358), TRUE);

  label11359 = gtk_label_new (_("QEI"));
  gtk_widget_show (label11359);
  gtk_frame_set_label_widget (GTK_FRAME (frame334), label11359);
  gtk_label_set_use_markup (GTK_LABEL (label11359), TRUE);

  frame336 = gtk_frame_new (NULL);
  gtk_widget_show (frame336);
  gtk_table_attach (GTK_TABLE (table40), frame336, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment482 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment482);
  gtk_container_add (GTK_CONTAINER (frame336), alignment482);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment482), 0, 0, 12, 0);

  alignment483 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment483);
  gtk_container_add (GTK_CONTAINER (alignment482), alignment483);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment483), 8, 8, 5, 0);

  hbox380 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox380);
  gtk_container_add (GTK_CONTAINER (alignment483), hbox380);

  label11360 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11360);
  gtk_box_pack_start (GTK_BOX (hbox380), label11360, FALSE, FALSE, 0);

  scrolledwindow187 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow187);
  gtk_box_pack_start (GTK_BOX (hbox380), scrolledwindow187, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow187), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow187), GTK_SHADOW_IN);

  textview144 = gtk_text_view_new ();
  gtk_widget_show (textview144);
  gtk_container_add (GTK_CONTAINER (scrolledwindow187), textview144);

  frame337 = gtk_frame_new (NULL);
  gtk_widget_show (frame337);
  gtk_box_pack_start (GTK_BOX (hbox380), frame337, FALSE, FALSE, 0);

  alignment484 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment484);
  gtk_container_add (GTK_CONTAINER (frame337), alignment484);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment484), 0, 0, 12, 0);

  hbox381 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox381);
  gtk_container_add (GTK_CONTAINER (alignment484), hbox381);

  entry86 = gtk_entry_new ();
  gtk_widget_show (entry86);
  gtk_box_pack_start (GTK_BOX (hbox381), entry86, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry86, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry86), 8226);

  button191 = gtk_button_new ();
  gtk_widget_show (button191);
  gtk_box_pack_start (GTK_BOX (hbox381), button191, FALSE, FALSE, 0);

  image235 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image235);
  gtk_container_add (GTK_CONTAINER (button191), image235);

  label11361 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11361);
  gtk_frame_set_label_widget (GTK_FRAME (frame337), label11361);
  gtk_label_set_use_markup (GTK_LABEL (label11361), TRUE);

  label11362 = gtk_label_new (_("DCM"));
  gtk_widget_show (label11362);
  gtk_frame_set_label_widget (GTK_FRAME (frame336), label11362);
  gtk_label_set_use_markup (GTK_LABEL (label11362), TRUE);

  frame338 = gtk_frame_new (NULL);
  gtk_widget_show (frame338);
  gtk_table_attach (GTK_TABLE (table40), frame338, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_frame_set_shadow_type (GTK_FRAME (frame338), GTK_SHADOW_NONE);

  alignment485 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment485);
  gtk_container_add (GTK_CONTAINER (frame338), alignment485);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment485), 0, 0, 12, 0);

  drawingarea14 = gtk_drawing_area_new ();
  gtk_widget_show (drawingarea14);
  gtk_container_add (GTK_CONTAINER (alignment485), drawingarea14);

  label11363 = gtk_label_new ("");
  gtk_widget_show (label11363);
  gtk_frame_set_label_widget (GTK_FRAME (frame338), label11363);
  gtk_label_set_use_markup (GTK_LABEL (label11363), TRUE);

  frame339 = gtk_frame_new (NULL);
  gtk_widget_show (frame339);
  gtk_table_attach (GTK_TABLE (table40), frame339, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment486 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment486);
  gtk_container_add (GTK_CONTAINER (frame339), alignment486);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment486), 0, 0, 12, 0);

  alignment487 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment487);
  gtk_container_add (GTK_CONTAINER (alignment486), alignment487);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment487), 8, 8, 5, 0);

  hbox382 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox382);
  gtk_container_add (GTK_CONTAINER (alignment487), hbox382);

  label11364 = gtk_label_new (_("Valeur"));
  gtk_widget_show (label11364);
  gtk_box_pack_start (GTK_BOX (hbox382), label11364, FALSE, FALSE, 0);

  scrolledwindow188 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow188);
  gtk_box_pack_start (GTK_BOX (hbox382), scrolledwindow188, TRUE, TRUE, 8);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow188), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow188), GTK_SHADOW_IN);

  textview145 = gtk_text_view_new ();
  gtk_widget_show (textview145);
  gtk_container_add (GTK_CONTAINER (scrolledwindow188), textview145);

  frame340 = gtk_frame_new (NULL);
  gtk_widget_show (frame340);
  gtk_box_pack_start (GTK_BOX (hbox382), frame340, FALSE, FALSE, 0);

  alignment488 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment488);
  gtk_container_add (GTK_CONTAINER (frame340), alignment488);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment488), 0, 0, 12, 0);

  hbox383 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox383);
  gtk_container_add (GTK_CONTAINER (alignment488), hbox383);

  entry87 = gtk_entry_new ();
  gtk_widget_show (entry87);
  gtk_box_pack_start (GTK_BOX (hbox383), entry87, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry87, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry87), 8226);

  button192 = gtk_button_new ();
  gtk_widget_show (button192);
  gtk_box_pack_start (GTK_BOX (hbox383), button192, FALSE, FALSE, 0);

  image236 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image236);
  gtk_container_add (GTK_CONTAINER (button192), image236);

  label11365 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11365);
  gtk_frame_set_label_widget (GTK_FRAME (frame340), label11365);
  gtk_label_set_use_markup (GTK_LABEL (label11365), TRUE);

  label11366 = gtk_label_new (_("AX12"));
  gtk_widget_show (label11366);
  gtk_frame_set_label_widget (GTK_FRAME (frame339), label11366);
  gtk_label_set_use_markup (GTK_LABEL (label11366), TRUE);

  frame341 = gtk_frame_new (NULL);
  gtk_widget_show (frame341);
  gtk_table_attach (GTK_TABLE (table40), frame341, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment489 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment489);
  gtk_container_add (GTK_CONTAINER (frame341), alignment489);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment489), 0, 0, 12, 0);

  hbox384 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox384);
  gtk_container_add (GTK_CONTAINER (alignment489), hbox384);
  gtk_container_set_border_width (GTK_CONTAINER (hbox384), 5);

  frame342 = gtk_frame_new (NULL);
  gtk_widget_show (frame342);
  gtk_box_pack_start (GTK_BOX (hbox384), frame342, TRUE, TRUE, 0);
  gtk_widget_set_size_request (frame342, 17, -1);
  gtk_frame_set_shadow_type (GTK_FRAME (frame342), GTK_SHADOW_NONE);

  alignment490 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment490);
  gtk_container_add (GTK_CONTAINER (frame342), alignment490);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment490), 0, 0, 12, 0);

  scrolledwindow189 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow189);
  gtk_container_add (GTK_CONTAINER (alignment490), scrolledwindow189);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow189), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow189), GTK_SHADOW_IN);

  textview146 = gtk_text_view_new ();
  gtk_widget_show (textview146);
  gtk_container_add (GTK_CONTAINER (scrolledwindow189), textview146);

  label11367 = gtk_label_new (_("N\302\260 ADC"));
  gtk_widget_show (label11367);
  gtk_frame_set_label_widget (GTK_FRAME (frame342), label11367);
  gtk_label_set_use_markup (GTK_LABEL (label11367), TRUE);

  frame343 = gtk_frame_new (NULL);
  gtk_widget_show (frame343);
  gtk_box_pack_start (GTK_BOX (hbox384), frame343, FALSE, FALSE, 0);
  gtk_frame_set_label_align (GTK_FRAME (frame343), 0.01, 0.5);

  alignment491 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment491);
  gtk_container_add (GTK_CONTAINER (frame343), alignment491);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment491), 0, 0, 12, 0);

  hbox385 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox385);
  gtk_container_add (GTK_CONTAINER (alignment491), hbox385);

  entry88 = gtk_entry_new ();
  gtk_widget_show (entry88);
  gtk_box_pack_start (GTK_BOX (hbox385), entry88, TRUE, TRUE, 0);
  gtk_widget_set_size_request (entry88, 71, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (entry88), 8226);

  button193 = gtk_button_new ();
  gtk_widget_show (button193);
  gtk_box_pack_start (GTK_BOX (hbox385), button193, FALSE, TRUE, 0);

  image237 = gtk_image_new_from_stock ("gtk-apply", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image237);
  gtk_container_add (GTK_CONTAINER (button193), image237);

  label11368 = gtk_label_new (_("For\303\247age      "));
  gtk_widget_show (label11368);
  gtk_frame_set_label_widget (GTK_FRAME (frame343), label11368);
  gtk_widget_set_size_request (label11368, -1, 19);
  gtk_label_set_use_markup (GTK_LABEL (label11368), TRUE);

  label11369 = gtk_label_new (_("ADC"));
  gtk_widget_show (label11369);
  gtk_frame_set_label_widget (GTK_FRAME (frame341), label11369);
  gtk_label_set_use_markup (GTK_LABEL (label11369), TRUE);

  frame344 = gtk_frame_new (NULL);
  gtk_widget_show (frame344);
  gtk_table_attach (GTK_TABLE (table40), frame344, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  alignment492 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment492);
  gtk_container_add (GTK_CONTAINER (frame344), alignment492);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment492), 0, 0, 12, 0);

  alignment493 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment493);
  gtk_container_add (GTK_CONTAINER (alignment492), alignment493);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment493), 0, 0, 12, 0);

  hbox386 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox386);
  gtk_container_add (GTK_CONTAINER (alignment493), hbox386);

  label11370 = gtk_label_new (_("Valeur       "));
  gtk_widget_show (label11370);
  gtk_box_pack_start (GTK_BOX (hbox386), label11370, FALSE, FALSE, 0);

  scrolledwindow190 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow190);
  gtk_box_pack_start (GTK_BOX (hbox386), scrolledwindow190, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow190), 25);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow190), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow190), GTK_SHADOW_IN);

  textview147 = gtk_text_view_new ();
  gtk_widget_show (textview147);
  gtk_container_add (GTK_CONTAINER (scrolledwindow190), textview147);
  gtk_widget_set_size_request (textview147, -1, 8);

  label11371 = gtk_label_new (_("PWM"));
  gtk_widget_show (label11371);
  gtk_frame_set_label_widget (GTK_FRAME (frame344), label11371);
  gtk_label_set_use_markup (GTK_LABEL (label11371), TRUE);

  label53 = gtk_label_new (_("Balises"));
  gtk_widget_show (label53);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (cei_ong_all), gtk_notebook_get_nth_page (GTK_NOTEBOOK (cei_ong_all), 4), label53);

  vbox10 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox10);
  gtk_box_pack_start (GTK_BOX (hbox3), vbox10, TRUE, TRUE, 0);
  gtk_widget_set_size_request (vbox10, 120, -1);

  vbox22 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox22);
  gtk_box_pack_start (GTK_BOX (vbox10), vbox22, TRUE, TRUE, 0);

  toolbar7 = gtk_toolbar_new ();
  gtk_widget_show (toolbar7);
  gtk_box_pack_start (GTK_BOX (vbox22), toolbar7, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar7), GTK_TOOLBAR_BOTH_HORIZ);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar7));

  toolitem31 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem31);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (toolitem31), TRUE);
  gtk_container_add (GTK_CONTAINER (toolbar7), toolitem31);

  uart_cb_cb1 = gtk_combo_box_entry_new_text ();
  gtk_widget_show (uart_cb_cb1);
  gtk_container_add (GTK_CONTAINER (toolitem31), uart_cb_cb1);
  gtk_widget_set_size_request (uart_cb_cb1, 136, -1);
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Actionneurs Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Actionneurs Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Balise Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Balise Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Propulsion Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Propulsion Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Strategie Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Strategie Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Supervision Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb1), _("Supervision Uart 2"));

  toolitem33 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem33);
  gtk_container_add (GTK_CONTAINER (toolbar7), toolitem33);

  uart_button_clear1 = gtk_button_new_with_mnemonic (_("Clear"));
  gtk_widget_show (uart_button_clear1);
  gtk_container_add (GTK_CONTAINER (toolitem33), uart_button_clear1);

  toolitem34 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem34);
  gtk_container_add (GTK_CONTAINER (toolbar7), toolitem34);

  uart_chk_keep1 = gtk_check_button_new_with_mnemonic (_("Garder"));
  gtk_widget_show (uart_chk_keep1);
  gtk_container_add (GTK_CONTAINER (toolitem34), uart_chk_keep1);

  scrolledwindow9 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow9);
  gtk_box_pack_start (GTK_BOX (vbox22), scrolledwindow9, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow9), GTK_SHADOW_IN);

  uart_zt_uart1 = gtk_text_view_new ();
  gtk_widget_show (uart_zt_uart1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow9), uart_zt_uart1);

  vbox23 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox23);
  gtk_box_pack_start (GTK_BOX (vbox10), vbox23, TRUE, TRUE, 0);

  toolbar8 = gtk_toolbar_new ();
  gtk_widget_show (toolbar8);
  gtk_box_pack_start (GTK_BOX (vbox23), toolbar8, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar8), GTK_TOOLBAR_BOTH_HORIZ);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar8));

  toolitem38 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem38);
  gtk_tool_item_set_expand (GTK_TOOL_ITEM (toolitem38), TRUE);
  gtk_container_add (GTK_CONTAINER (toolbar8), toolitem38);

  uart_cb_cb2 = gtk_combo_box_entry_new_text ();
  gtk_widget_show (uart_cb_cb2);
  gtk_container_add (GTK_CONTAINER (toolitem38), uart_cb_cb2);
  gtk_widget_set_size_request (uart_cb_cb2, 136, -1);
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Actionneurs Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Actionneurs Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Balise Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Balise Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Propulsion Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Propulsion Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Strategie Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Strategie Uart 2"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Supervision Uart 1"));
  gtk_combo_box_append_text (GTK_COMBO_BOX (uart_cb_cb2), _("Supervision Uart 2"));

  toolitem39 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem39);
  gtk_container_add (GTK_CONTAINER (toolbar8), toolitem39);

  uart_button_clear2 = gtk_button_new_with_mnemonic (_("Clear"));
  gtk_widget_show (uart_button_clear2);
  gtk_container_add (GTK_CONTAINER (toolitem39), uart_button_clear2);

  toolitem40 = (GtkWidget*) gtk_tool_item_new ();
  gtk_widget_show (toolitem40);
  gtk_container_add (GTK_CONTAINER (toolbar8), toolitem40);

  uart_chk_keep2 = gtk_check_button_new_with_mnemonic (_("Garder"));
  gtk_widget_show (uart_chk_keep2);
  gtk_container_add (GTK_CONTAINER (toolitem40), uart_chk_keep2);

  scrolledwindow10 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow10);
  gtk_box_pack_start (GTK_BOX (vbox23), scrolledwindow10, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow10), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow10), GTK_SHADOW_IN);

  uart_zt_uart2 = gtk_text_view_new ();
  gtk_widget_show (uart_zt_uart2);
  gtk_container_add (GTK_CONTAINER (scrolledwindow10), uart_zt_uart2);

  vbox24 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox24);
  gtk_box_pack_start (GTK_BOX (vbox10), vbox24, TRUE, TRUE, 0);

  label28 = gtk_label_new (_("<b>Terminal Utilisateur</b>"));
  gtk_widget_show (label28);
  gtk_box_pack_start (GTK_BOX (vbox24), label28, FALSE, FALSE, 0);
  gtk_label_set_use_markup (GTK_LABEL (label28), TRUE);

  scrolledwindow153 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow153);
  gtk_box_pack_start (GTK_BOX (vbox24), scrolledwindow153, TRUE, TRUE, 0);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow153), GTK_SHADOW_IN);

  terminal_zt_user = gtk_text_view_new ();
  gtk_widget_show (terminal_zt_user);
  gtk_container_add (GTK_CONTAINER (scrolledwindow153), terminal_zt_user);

  g_signal_connect ((gpointer) nouveau1, "activate",
                    G_CALLBACK (on_nouveau1_activate),
                    NULL);
  g_signal_connect ((gpointer) ouvrir1, "activate",
                    G_CALLBACK (on_ouvrir1_activate),
                    NULL);
  g_signal_connect ((gpointer) enregistrer1, "activate",
                    G_CALLBACK (on_enregistrer1_activate),
                    NULL);
  g_signal_connect ((gpointer) enregistrer_sous1, "activate",
                    G_CALLBACK (on_enregistrer_sous1_activate),
                    NULL);
  g_signal_connect ((gpointer) quitter1, "activate",
                    G_CALLBACK (on_quitter1_activate),
                    NULL);
  g_signal_connect ((gpointer) couper1, "activate",
                    G_CALLBACK (on_couper1_activate),
                    NULL);
  g_signal_connect ((gpointer) copier1, "activate",
                    G_CALLBACK (on_copier1_activate),
                    NULL);
  g_signal_connect ((gpointer) coller1, "activate",
                    G_CALLBACK (on_coller1_activate),
                    NULL);
  g_signal_connect ((gpointer) supprimer1, "activate",
                    G_CALLBACK (on_supprimer1_activate),
                    NULL);
  g_signal_connect ((gpointer) ___propos1, "activate",
                    G_CALLBACK (on____propos1_activate),
                    NULL);
  g_signal_connect ((gpointer) can_button_clear, "clicked",
                    G_CALLBACK (on_can_button_clear_clicked),
                    NULL);
  g_signal_connect ((gpointer) can_msg_button_bt1, "clicked",
                    G_CALLBACK (on_can_msg_button_bt1_clicked),
                    NULL);
  g_signal_connect ((gpointer) can_send_button_add, "clicked",
                    G_CALLBACK (on_can_send_button_add_clicked),
                    NULL);
  g_signal_connect ((gpointer) can_send_button_send, "clicked",
                    G_CALLBACK (on_can_send_button_send_clicked),
                    NULL);
  g_signal_connect ((gpointer) can_filt_chk_all, "toggled",
                    G_CALLBACK (on_can_filt_chk_all_toggled),
                    NULL);
  g_signal_connect ((gpointer) can_filt_chk_bal, "toggled",
                    G_CALLBACK (on_can_filt_chk_bal_toggled),
                    NULL);
  g_signal_connect ((gpointer) can_filt_chk_sup, "toggled",
                    G_CALLBACK (on_can_filt_chk_sup_toggled),
                    NULL);
  g_signal_connect ((gpointer) can_filt_chk_stra, "toggled",
                    G_CALLBACK (on_can_filt_chk_stra_toggled),
                    NULL);
  g_signal_connect ((gpointer) can_filt_chk_pro, "toggled",
                    G_CALLBACK (on_can_filt_chk_pro_toggled),
                    NULL);
  g_signal_connect ((gpointer) can_filt_chk_act, "toggled",
                    G_CALLBACK (on_can_filt_chk_act_toggled),
                    NULL);
  g_signal_connect ((gpointer) can_filt_button_ok, "clicked",
                    G_CALLBACK (on_can_filt_button_ok_clicked),
                    NULL);
  g_signal_connect ((gpointer) os_button_play, "clicked",
                    G_CALLBACK (on_os_button_play_clicked),
                    NULL);
  g_signal_connect ((gpointer) os_button_pause, "clicked",
                    G_CALLBACK (on_os_button_pause_clicked),
                    NULL);
  g_signal_connect ((gpointer) os_button_stop, "clicked",
                    G_CALLBACK (on_os_button_stop_clicked),
                    NULL);
  g_signal_connect ((gpointer) os_button_jumpOk, "clicked",
                    G_CALLBACK (on_os_button_jumpOk_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_act_button_bt1, "clicked",
                    G_CALLBACK (on_cei_act_button_bt1_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_act_button_bt2, "clicked",
                    G_CALLBACK (on_cei_act_button_bt2_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_act_button_bt3, "clicked",
                    G_CALLBACK (on_cei_act_button_bt3_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_act_button_bt4, "clicked",
                    G_CALLBACK (on_cei_act_button_bt4_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_act_button_reset, "clicked",
                    G_CALLBACK (on_cei_act_button_reset_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_pro_button_bt1, "clicked",
                    G_CALLBACK (on_cei_pro_button_bt1_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_pro_button_bt2, "clicked",
                    G_CALLBACK (on_cei_pro_button_bt2_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_pro_button_bt3, "clicked",
                    G_CALLBACK (on_cei_pro_button_bt3_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_pro_button_bt4, "clicked",
                    G_CALLBACK (on_cei_pro_button_bt4_clicked),
                    NULL);
  g_signal_connect ((gpointer) button158, "clicked",
                    G_CALLBACK (on_cei_pro_button_reset_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_stra_button_bt1, "clicked",
                    G_CALLBACK (on_cei_stra_button_bt1_clicked),
                    NULL);
  g_signal_connect ((gpointer) button165, "clicked",
                    G_CALLBACK (on_cei_stra_button_bt2_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_stra_button_bt3, "clicked",
                    G_CALLBACK (on_cei_stra_button_bt3_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_stra_button_bt4, "clicked",
                    G_CALLBACK (on_cei_stra_button_bt4_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_stra_button_reset, "clicked",
                    G_CALLBACK (on_cei_stra_button_reset_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_sup_button_bt1, "clicked",
                    G_CALLBACK (on_cei_sup_button_bt1_clicked),
                    NULL);
  g_signal_connect ((gpointer) cei_sup_button_bt2, "clicked",
                    G_CALLBACK (on_cei_sup_button_bt2_clicked),
                    NULL);
  g_signal_connect ((gpointer) button176, "clicked",
                    G_CALLBACK (on_cei_sup_button_bt3_clicked),
                    NULL);
  g_signal_connect ((gpointer) button177, "clicked",
                    G_CALLBACK (on_cei_sup_button_bt4_clicked),
                    NULL);
  g_signal_connect ((gpointer) button178, "clicked",
                    G_CALLBACK (on_cei_sup_button_reset_clicked),
                    NULL);
  g_signal_connect ((gpointer) button185, "clicked",
                    G_CALLBACK (on_cei_act_button_bt2_clicked),
                    NULL);
  g_signal_connect ((gpointer) button186, "clicked",
                    G_CALLBACK (on_cei_act_button_bt3_clicked),
                    NULL);
  g_signal_connect ((gpointer) button187, "clicked",
                    G_CALLBACK (on_cei_act_button_bt4_clicked),
                    NULL);
  g_signal_connect ((gpointer) button188, "clicked",
                    G_CALLBACK (on_cei_act_button_reset_clicked),
                    NULL);
  g_signal_connect ((gpointer) uart_cb_cb1, "changed",
                    G_CALLBACK (on_uart_cb_cb1_changed),
                    NULL);
  g_signal_connect ((gpointer) uart_button_clear1, "clicked",
                    G_CALLBACK (on_uart_button_clear1_clicked),
                    NULL);
  g_signal_connect ((gpointer) uart_chk_keep1, "toggled",
                    G_CALLBACK (on_uart_chk_keep1_toggled),
                    NULL);
  g_signal_connect ((gpointer) uart_cb_cb2, "changed",
                    G_CALLBACK (on_uart_cb_cb2_changed),
                    NULL);
  g_signal_connect ((gpointer) uart_button_clear2, "clicked",
                    G_CALLBACK (on_uart_button_clear2_clicked),
                    NULL);
  g_signal_connect ((gpointer) uart_chk_keep2, "toggled",
                    G_CALLBACK (on_uart_chk_keep2_toggled),
                    NULL);

  atko = gtk_widget_get_accessible (label35);
  atk_object_set_name (atko, _("Message"));


  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (Fenetre, Fenetre, "Fenetre");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (Fenetre, menubar1, "menubar1");
  GLADE_HOOKUP_OBJECT (Fenetre, menuitem10, "menuitem10");
  GLADE_HOOKUP_OBJECT (Fenetre, menu10, "menu10");
  GLADE_HOOKUP_OBJECT (Fenetre, nouveau1, "nouveau1");
  GLADE_HOOKUP_OBJECT (Fenetre, ouvrir1, "ouvrir1");
  GLADE_HOOKUP_OBJECT (Fenetre, enregistrer1, "enregistrer1");
  GLADE_HOOKUP_OBJECT (Fenetre, enregistrer_sous1, "enregistrer_sous1");
  GLADE_HOOKUP_OBJECT (Fenetre, separatormenuitem1, "separatormenuitem1");
  GLADE_HOOKUP_OBJECT (Fenetre, quitter1, "quitter1");
  GLADE_HOOKUP_OBJECT (Fenetre, menuitem11, "menuitem11");
  GLADE_HOOKUP_OBJECT (Fenetre, menu11, "menu11");
  GLADE_HOOKUP_OBJECT (Fenetre, couper1, "couper1");
  GLADE_HOOKUP_OBJECT (Fenetre, copier1, "copier1");
  GLADE_HOOKUP_OBJECT (Fenetre, coller1, "coller1");
  GLADE_HOOKUP_OBJECT (Fenetre, supprimer1, "supprimer1");
  GLADE_HOOKUP_OBJECT (Fenetre, menuitem12, "menuitem12");
  GLADE_HOOKUP_OBJECT (Fenetre, menu12, "menu12");
  GLADE_HOOKUP_OBJECT (Fenetre, menuitem13, "menuitem13");
  GLADE_HOOKUP_OBJECT (Fenetre, menu13, "menu13");
  GLADE_HOOKUP_OBJECT (Fenetre, ___propos1, "___propos1");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox7, "vbox7");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox8, "vbox8");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (Fenetre, label14, "label14");
  GLADE_HOOKUP_OBJECT (Fenetre, can_button_clear, "can_button_clear");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox9, "vbox9");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow2, "scrolledwindow2");
  GLADE_HOOKUP_OBJECT (Fenetre, can_zt_main, "can_zt_main");
  GLADE_HOOKUP_OBJECT (Fenetre, notebook2, "notebook2");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow11, "scrolledwindow11");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport1, "viewport1");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox27, "vbox27");
  GLADE_HOOKUP_OBJECT (Fenetre, table5, "table5");
  GLADE_HOOKUP_OBJECT (Fenetre, label34, "label34");
  GLADE_HOOKUP_OBJECT (Fenetre, label35, "label35");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow12, "scrolledwindow12");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport2, "viewport2");
  GLADE_HOOKUP_OBJECT (Fenetre, table6, "table6");
  GLADE_HOOKUP_OBJECT (Fenetre, can_msg_button_bt1, "can_msg_button_bt1");
  GLADE_HOOKUP_OBJECT (Fenetre, button32, "button32");
  GLADE_HOOKUP_OBJECT (Fenetre, button33, "button33");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow13, "scrolledwindow13");
  GLADE_HOOKUP_OBJECT (Fenetre, can_msg_zt_desc1, "can_msg_zt_desc1");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow14, "scrolledwindow14");
  GLADE_HOOKUP_OBJECT (Fenetre, can_msg_zt_msg1, "can_msg_zt_msg1");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow15, "scrolledwindow15");
  GLADE_HOOKUP_OBJECT (Fenetre, textview11, "textview11");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow16, "scrolledwindow16");
  GLADE_HOOKUP_OBJECT (Fenetre, textview12, "textview12");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow17, "scrolledwindow17");
  GLADE_HOOKUP_OBJECT (Fenetre, textview13, "textview13");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow18, "scrolledwindow18");
  GLADE_HOOKUP_OBJECT (Fenetre, textview14, "textview14");
  GLADE_HOOKUP_OBJECT (Fenetre, label15, "label15");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox28, "vbox28");
  GLADE_HOOKUP_OBJECT (Fenetre, can_send_st_main, "can_send_st_main");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox9, "hbox9");
  GLADE_HOOKUP_OBJECT (Fenetre, can_send_button_add, "can_send_button_add");
  GLADE_HOOKUP_OBJECT (Fenetre, can_send_button_send, "can_send_button_send");
  GLADE_HOOKUP_OBJECT (Fenetre, label10, "label10");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox41, "vbox41");
  GLADE_HOOKUP_OBJECT (Fenetre, frame13, "frame13");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment28, "alignment28");
  GLADE_HOOKUP_OBJECT (Fenetre, table41, "table41");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_chk_all, "can_filt_chk_all");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_chk_bal, "can_filt_chk_bal");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_chk_sup, "can_filt_chk_sup");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_chk_stra, "can_filt_chk_stra");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_chk_pro, "can_filt_chk_pro");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_chk_act, "can_filt_chk_act");
  GLADE_HOOKUP_OBJECT (Fenetre, label48, "label48");
  GLADE_HOOKUP_OBJECT (Fenetre, frame14, "frame14");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment29, "alignment29");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox19, "hbox19");
  GLADE_HOOKUP_OBJECT (Fenetre, frame15, "frame15");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment30, "alignment30");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_st_sid, "can_filt_st_sid");
  GLADE_HOOKUP_OBJECT (Fenetre, label50, "label50");
  GLADE_HOOKUP_OBJECT (Fenetre, frame16, "frame16");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment31, "alignment31");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_st_data, "can_filt_st_data");
  GLADE_HOOKUP_OBJECT (Fenetre, label51, "label51");
  GLADE_HOOKUP_OBJECT (Fenetre, frame17, "frame17");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment32, "alignment32");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_st_taille, "can_filt_st_taille");
  GLADE_HOOKUP_OBJECT (Fenetre, label52, "label52");
  GLADE_HOOKUP_OBJECT (Fenetre, can_filt_button_ok, "can_filt_button_ok");
  GLADE_HOOKUP_OBJECT (Fenetre, image20, "image20");
  GLADE_HOOKUP_OBJECT (Fenetre, label49, "label49");
  GLADE_HOOKUP_OBJECT (Fenetre, label11, "label11");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox11, "vbox11");
  GLADE_HOOKUP_OBJECT (Fenetre, toolbar20, "toolbar20");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem104, "toolitem104");
  GLADE_HOOKUP_OBJECT (Fenetre, os_button_play, "os_button_play");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment300, "alignment300");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox191, "hbox191");
  GLADE_HOOKUP_OBJECT (Fenetre, image108, "image108");
  GLADE_HOOKUP_OBJECT (Fenetre, label339, "label339");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem105, "toolitem105");
  GLADE_HOOKUP_OBJECT (Fenetre, os_button_pause, "os_button_pause");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment301, "alignment301");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox192, "hbox192");
  GLADE_HOOKUP_OBJECT (Fenetre, image109, "image109");
  GLADE_HOOKUP_OBJECT (Fenetre, label340, "label340");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem106, "toolitem106");
  GLADE_HOOKUP_OBJECT (Fenetre, os_button_stop, "os_button_stop");
  GLADE_HOOKUP_OBJECT (Fenetre, image110, "image110");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem107, "toolitem107");
  GLADE_HOOKUP_OBJECT (Fenetre, frame199, "frame199");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment302, "alignment302");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox193, "hbox193");
  GLADE_HOOKUP_OBJECT (Fenetre, os_st_timeJump, "os_st_timeJump");
  GLADE_HOOKUP_OBJECT (Fenetre, os_button_jumpOk, "os_button_jumpOk");
  GLADE_HOOKUP_OBJECT (Fenetre, label341, "label341");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem108, "toolitem108");
  GLADE_HOOKUP_OBJECT (Fenetre, frame200, "frame200");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment303, "alignment303");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow114, "scrolledwindow114");
  GLADE_HOOKUP_OBJECT (Fenetre, os_zt_CurrentTime, "os_zt_CurrentTime");
  GLADE_HOOKUP_OBJECT (Fenetre, label342, "label342");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem109, "toolitem109");
  GLADE_HOOKUP_OBJECT (Fenetre, frame202, "frame202");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment305, "alignment305");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow116, "scrolledwindow116");
  GLADE_HOOKUP_OBJECT (Fenetre, os_zt_matchTime, "os_zt_matchTime");
  GLADE_HOOKUP_OBJECT (Fenetre, label350, "label350");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_ong_all, "cei_ong_all");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox102, "vbox102");
  GLADE_HOOKUP_OBJECT (Fenetre, table30, "table30");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow144, "scrolledwindow144");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport7, "viewport7");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox103, "vbox103");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox269, "hbox269");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox270, "hbox270");
  GLADE_HOOKUP_OBJECT (Fenetre, label11199, "label11199");
  GLADE_HOOKUP_OBJECT (Fenetre, image165, "image165");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox271, "hbox271");
  GLADE_HOOKUP_OBJECT (Fenetre, label11200, "label11200");
  GLADE_HOOKUP_OBJECT (Fenetre, image166, "image166");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox272, "hbox272");
  GLADE_HOOKUP_OBJECT (Fenetre, label11201, "label11201");
  GLADE_HOOKUP_OBJECT (Fenetre, image167, "image167");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox273, "hbox273");
  GLADE_HOOKUP_OBJECT (Fenetre, label11202, "label11202");
  GLADE_HOOKUP_OBJECT (Fenetre, image168, "image168");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox274, "hbox274");
  GLADE_HOOKUP_OBJECT (Fenetre, label11203, "label11203");
  GLADE_HOOKUP_OBJECT (Fenetre, image169, "image169");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox275, "hbox275");
  GLADE_HOOKUP_OBJECT (Fenetre, label11204, "label11204");
  GLADE_HOOKUP_OBJECT (Fenetre, image170, "image170");
  GLADE_HOOKUP_OBJECT (Fenetre, frame251, "frame251");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment369, "alignment369");
  GLADE_HOOKUP_OBJECT (Fenetre, image171, "image171");
  GLADE_HOOKUP_OBJECT (Fenetre, label11205, "label11205");
  GLADE_HOOKUP_OBJECT (Fenetre, hbuttonbox5, "hbuttonbox5");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_act_button_bt1, "cei_act_button_bt1");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_act_button_bt2, "cei_act_button_bt2");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_act_button_bt3, "cei_act_button_bt3");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_act_button_bt4, "cei_act_button_bt4");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_act_button_reset, "cei_act_button_reset");
  GLADE_HOOKUP_OBJECT (Fenetre, image172, "image172");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox104, "vbox104");
  GLADE_HOOKUP_OBJECT (Fenetre, table31, "table31");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox105, "vbox105");
  GLADE_HOOKUP_OBJECT (Fenetre, frame252, "frame252");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment370, "alignment370");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox276, "hbox276");
  GLADE_HOOKUP_OBJECT (Fenetre, label11206, "label11206");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow145, "scrolledwindow145");
  GLADE_HOOKUP_OBJECT (Fenetre, textview107, "textview107");
  GLADE_HOOKUP_OBJECT (Fenetre, label11207, "label11207");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox106, "vbox106");
  GLADE_HOOKUP_OBJECT (Fenetre, frame253, "frame253");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment371, "alignment371");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment372, "alignment372");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox277, "hbox277");
  GLADE_HOOKUP_OBJECT (Fenetre, label11208, "label11208");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow146, "scrolledwindow146");
  GLADE_HOOKUP_OBJECT (Fenetre, textview108, "textview108");
  GLADE_HOOKUP_OBJECT (Fenetre, frame254, "frame254");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment373, "alignment373");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox278, "hbox278");
  GLADE_HOOKUP_OBJECT (Fenetre, entry64, "entry64");
  GLADE_HOOKUP_OBJECT (Fenetre, button144, "button144");
  GLADE_HOOKUP_OBJECT (Fenetre, image173, "image173");
  GLADE_HOOKUP_OBJECT (Fenetre, label11209, "label11209");
  GLADE_HOOKUP_OBJECT (Fenetre, label11210, "label11210");
  GLADE_HOOKUP_OBJECT (Fenetre, frame255, "frame255");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment374, "alignment374");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment375, "alignment375");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox279, "hbox279");
  GLADE_HOOKUP_OBJECT (Fenetre, label11211, "label11211");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow147, "scrolledwindow147");
  GLADE_HOOKUP_OBJECT (Fenetre, textview109, "textview109");
  GLADE_HOOKUP_OBJECT (Fenetre, label11212, "label11212");
  GLADE_HOOKUP_OBJECT (Fenetre, frame256, "frame256");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment376, "alignment376");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox280, "hbox280");
  GLADE_HOOKUP_OBJECT (Fenetre, label11213, "label11213");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow148, "scrolledwindow148");
  GLADE_HOOKUP_OBJECT (Fenetre, textview110, "textview110");
  GLADE_HOOKUP_OBJECT (Fenetre, frame257, "frame257");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment377, "alignment377");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox281, "hbox281");
  GLADE_HOOKUP_OBJECT (Fenetre, entry65, "entry65");
  GLADE_HOOKUP_OBJECT (Fenetre, button145, "button145");
  GLADE_HOOKUP_OBJECT (Fenetre, image174, "image174");
  GLADE_HOOKUP_OBJECT (Fenetre, label11214, "label11214");
  GLADE_HOOKUP_OBJECT (Fenetre, label11215, "label11215");
  GLADE_HOOKUP_OBJECT (Fenetre, frame258, "frame258");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment378, "alignment378");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment379, "alignment379");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox282, "hbox282");
  GLADE_HOOKUP_OBJECT (Fenetre, label11216, "label11216");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow149, "scrolledwindow149");
  GLADE_HOOKUP_OBJECT (Fenetre, textview111, "textview111");
  GLADE_HOOKUP_OBJECT (Fenetre, frame259, "frame259");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment380, "alignment380");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox283, "hbox283");
  GLADE_HOOKUP_OBJECT (Fenetre, entry66, "entry66");
  GLADE_HOOKUP_OBJECT (Fenetre, button146, "button146");
  GLADE_HOOKUP_OBJECT (Fenetre, image175, "image175");
  GLADE_HOOKUP_OBJECT (Fenetre, label11217, "label11217");
  GLADE_HOOKUP_OBJECT (Fenetre, label11218, "label11218");
  GLADE_HOOKUP_OBJECT (Fenetre, frame260, "frame260");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment381, "alignment381");
  GLADE_HOOKUP_OBJECT (Fenetre, drawingarea10, "drawingarea10");
  GLADE_HOOKUP_OBJECT (Fenetre, label11219, "label11219");
  GLADE_HOOKUP_OBJECT (Fenetre, frame261, "frame261");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment382, "alignment382");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment383, "alignment383");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox284, "hbox284");
  GLADE_HOOKUP_OBJECT (Fenetre, label11220, "label11220");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow150, "scrolledwindow150");
  GLADE_HOOKUP_OBJECT (Fenetre, textview112, "textview112");
  GLADE_HOOKUP_OBJECT (Fenetre, frame262, "frame262");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment384, "alignment384");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox285, "hbox285");
  GLADE_HOOKUP_OBJECT (Fenetre, entry67, "entry67");
  GLADE_HOOKUP_OBJECT (Fenetre, button147, "button147");
  GLADE_HOOKUP_OBJECT (Fenetre, image176, "image176");
  GLADE_HOOKUP_OBJECT (Fenetre, label11221, "label11221");
  GLADE_HOOKUP_OBJECT (Fenetre, label11222, "label11222");
  GLADE_HOOKUP_OBJECT (Fenetre, frame263, "frame263");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment385, "alignment385");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox286, "hbox286");
  GLADE_HOOKUP_OBJECT (Fenetre, frame264, "frame264");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment386, "alignment386");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow151, "scrolledwindow151");
  GLADE_HOOKUP_OBJECT (Fenetre, textview113, "textview113");
  GLADE_HOOKUP_OBJECT (Fenetre, label11223, "label11223");
  GLADE_HOOKUP_OBJECT (Fenetre, frame265, "frame265");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment387, "alignment387");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox287, "hbox287");
  GLADE_HOOKUP_OBJECT (Fenetre, entry68, "entry68");
  GLADE_HOOKUP_OBJECT (Fenetre, button148, "button148");
  GLADE_HOOKUP_OBJECT (Fenetre, image177, "image177");
  GLADE_HOOKUP_OBJECT (Fenetre, label11224, "label11224");
  GLADE_HOOKUP_OBJECT (Fenetre, label11225, "label11225");
  GLADE_HOOKUP_OBJECT (Fenetre, frame266, "frame266");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment388, "alignment388");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment389, "alignment389");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox288, "hbox288");
  GLADE_HOOKUP_OBJECT (Fenetre, label11226, "label11226");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow152, "scrolledwindow152");
  GLADE_HOOKUP_OBJECT (Fenetre, textview114, "textview114");
  GLADE_HOOKUP_OBJECT (Fenetre, label11227, "label11227");
  GLADE_HOOKUP_OBJECT (Fenetre, label333, "label333");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox110, "vbox110");
  GLADE_HOOKUP_OBJECT (Fenetre, table33, "table33");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow155, "scrolledwindow155");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport8, "viewport8");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox111, "vbox111");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox307, "hbox307");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox308, "hbox308");
  GLADE_HOOKUP_OBJECT (Fenetre, label11256, "label11256");
  GLADE_HOOKUP_OBJECT (Fenetre, image186, "image186");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox309, "hbox309");
  GLADE_HOOKUP_OBJECT (Fenetre, label11257, "label11257");
  GLADE_HOOKUP_OBJECT (Fenetre, image187, "image187");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox310, "hbox310");
  GLADE_HOOKUP_OBJECT (Fenetre, label11258, "label11258");
  GLADE_HOOKUP_OBJECT (Fenetre, image188, "image188");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox311, "hbox311");
  GLADE_HOOKUP_OBJECT (Fenetre, label11259, "label11259");
  GLADE_HOOKUP_OBJECT (Fenetre, image189, "image189");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox312, "hbox312");
  GLADE_HOOKUP_OBJECT (Fenetre, label11260, "label11260");
  GLADE_HOOKUP_OBJECT (Fenetre, image190, "image190");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox313, "hbox313");
  GLADE_HOOKUP_OBJECT (Fenetre, label11261, "label11261");
  GLADE_HOOKUP_OBJECT (Fenetre, image191, "image191");
  GLADE_HOOKUP_OBJECT (Fenetre, frame281, "frame281");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment410, "alignment410");
  GLADE_HOOKUP_OBJECT (Fenetre, image192, "image192");
  GLADE_HOOKUP_OBJECT (Fenetre, label11262, "label11262");
  GLADE_HOOKUP_OBJECT (Fenetre, hbuttonbox6, "hbuttonbox6");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_pro_button_bt1, "cei_pro_button_bt1");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_pro_button_bt2, "cei_pro_button_bt2");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_pro_button_bt3, "cei_pro_button_bt3");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_pro_button_bt4, "cei_pro_button_bt4");
  GLADE_HOOKUP_OBJECT (Fenetre, button158, "button158");
  GLADE_HOOKUP_OBJECT (Fenetre, image193, "image193");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox112, "vbox112");
  GLADE_HOOKUP_OBJECT (Fenetre, table34, "table34");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox113, "vbox113");
  GLADE_HOOKUP_OBJECT (Fenetre, frame282, "frame282");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment411, "alignment411");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox314, "hbox314");
  GLADE_HOOKUP_OBJECT (Fenetre, label11263, "label11263");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow156, "scrolledwindow156");
  GLADE_HOOKUP_OBJECT (Fenetre, textview116, "textview116");
  GLADE_HOOKUP_OBJECT (Fenetre, label11264, "label11264");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox114, "vbox114");
  GLADE_HOOKUP_OBJECT (Fenetre, frame283, "frame283");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment412, "alignment412");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment413, "alignment413");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox315, "hbox315");
  GLADE_HOOKUP_OBJECT (Fenetre, label11265, "label11265");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow157, "scrolledwindow157");
  GLADE_HOOKUP_OBJECT (Fenetre, textview117, "textview117");
  GLADE_HOOKUP_OBJECT (Fenetre, frame284, "frame284");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment414, "alignment414");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox316, "hbox316");
  GLADE_HOOKUP_OBJECT (Fenetre, entry69, "entry69");
  GLADE_HOOKUP_OBJECT (Fenetre, button159, "button159");
  GLADE_HOOKUP_OBJECT (Fenetre, image194, "image194");
  GLADE_HOOKUP_OBJECT (Fenetre, label11266, "label11266");
  GLADE_HOOKUP_OBJECT (Fenetre, label11267, "label11267");
  GLADE_HOOKUP_OBJECT (Fenetre, frame285, "frame285");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment415, "alignment415");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment416, "alignment416");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox317, "hbox317");
  GLADE_HOOKUP_OBJECT (Fenetre, label11268, "label11268");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow158, "scrolledwindow158");
  GLADE_HOOKUP_OBJECT (Fenetre, textview118, "textview118");
  GLADE_HOOKUP_OBJECT (Fenetre, label11269, "label11269");
  GLADE_HOOKUP_OBJECT (Fenetre, frame286, "frame286");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment417, "alignment417");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox318, "hbox318");
  GLADE_HOOKUP_OBJECT (Fenetre, label11270, "label11270");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow159, "scrolledwindow159");
  GLADE_HOOKUP_OBJECT (Fenetre, textview119, "textview119");
  GLADE_HOOKUP_OBJECT (Fenetre, frame287, "frame287");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment418, "alignment418");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox319, "hbox319");
  GLADE_HOOKUP_OBJECT (Fenetre, entry70, "entry70");
  GLADE_HOOKUP_OBJECT (Fenetre, button160, "button160");
  GLADE_HOOKUP_OBJECT (Fenetre, image195, "image195");
  GLADE_HOOKUP_OBJECT (Fenetre, label11271, "label11271");
  GLADE_HOOKUP_OBJECT (Fenetre, label11272, "label11272");
  GLADE_HOOKUP_OBJECT (Fenetre, frame288, "frame288");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment419, "alignment419");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment420, "alignment420");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox320, "hbox320");
  GLADE_HOOKUP_OBJECT (Fenetre, label11273, "label11273");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow160, "scrolledwindow160");
  GLADE_HOOKUP_OBJECT (Fenetre, textview120, "textview120");
  GLADE_HOOKUP_OBJECT (Fenetre, frame289, "frame289");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment421, "alignment421");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox321, "hbox321");
  GLADE_HOOKUP_OBJECT (Fenetre, entry71, "entry71");
  GLADE_HOOKUP_OBJECT (Fenetre, button161, "button161");
  GLADE_HOOKUP_OBJECT (Fenetre, image196, "image196");
  GLADE_HOOKUP_OBJECT (Fenetre, label11274, "label11274");
  GLADE_HOOKUP_OBJECT (Fenetre, label11275, "label11275");
  GLADE_HOOKUP_OBJECT (Fenetre, frame290, "frame290");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment422, "alignment422");
  GLADE_HOOKUP_OBJECT (Fenetre, drawingarea11, "drawingarea11");
  GLADE_HOOKUP_OBJECT (Fenetre, label11276, "label11276");
  GLADE_HOOKUP_OBJECT (Fenetre, frame291, "frame291");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment423, "alignment423");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment424, "alignment424");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox322, "hbox322");
  GLADE_HOOKUP_OBJECT (Fenetre, label11277, "label11277");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow161, "scrolledwindow161");
  GLADE_HOOKUP_OBJECT (Fenetre, textview121, "textview121");
  GLADE_HOOKUP_OBJECT (Fenetre, frame292, "frame292");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment425, "alignment425");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox323, "hbox323");
  GLADE_HOOKUP_OBJECT (Fenetre, entry72, "entry72");
  GLADE_HOOKUP_OBJECT (Fenetre, button162, "button162");
  GLADE_HOOKUP_OBJECT (Fenetre, image197, "image197");
  GLADE_HOOKUP_OBJECT (Fenetre, label11278, "label11278");
  GLADE_HOOKUP_OBJECT (Fenetre, label11279, "label11279");
  GLADE_HOOKUP_OBJECT (Fenetre, frame293, "frame293");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment426, "alignment426");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox324, "hbox324");
  GLADE_HOOKUP_OBJECT (Fenetre, frame294, "frame294");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment427, "alignment427");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow162, "scrolledwindow162");
  GLADE_HOOKUP_OBJECT (Fenetre, textview122, "textview122");
  GLADE_HOOKUP_OBJECT (Fenetre, label11280, "label11280");
  GLADE_HOOKUP_OBJECT (Fenetre, frame295, "frame295");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment428, "alignment428");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox325, "hbox325");
  GLADE_HOOKUP_OBJECT (Fenetre, entry73, "entry73");
  GLADE_HOOKUP_OBJECT (Fenetre, button163, "button163");
  GLADE_HOOKUP_OBJECT (Fenetre, image198, "image198");
  GLADE_HOOKUP_OBJECT (Fenetre, label11281, "label11281");
  GLADE_HOOKUP_OBJECT (Fenetre, label11282, "label11282");
  GLADE_HOOKUP_OBJECT (Fenetre, frame296, "frame296");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment429, "alignment429");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment430, "alignment430");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox326, "hbox326");
  GLADE_HOOKUP_OBJECT (Fenetre, label11283, "label11283");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow163, "scrolledwindow163");
  GLADE_HOOKUP_OBJECT (Fenetre, textview123, "textview123");
  GLADE_HOOKUP_OBJECT (Fenetre, label11284, "label11284");
  GLADE_HOOKUP_OBJECT (Fenetre, label12333, "label12333");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox115, "vbox115");
  GLADE_HOOKUP_OBJECT (Fenetre, table35, "table35");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow164, "scrolledwindow164");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport9, "viewport9");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox116, "vbox116");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox327, "hbox327");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox328, "hbox328");
  GLADE_HOOKUP_OBJECT (Fenetre, label11285, "label11285");
  GLADE_HOOKUP_OBJECT (Fenetre, image199, "image199");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox329, "hbox329");
  GLADE_HOOKUP_OBJECT (Fenetre, label11286, "label11286");
  GLADE_HOOKUP_OBJECT (Fenetre, image200, "image200");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox330, "hbox330");
  GLADE_HOOKUP_OBJECT (Fenetre, label11287, "label11287");
  GLADE_HOOKUP_OBJECT (Fenetre, image201, "image201");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox331, "hbox331");
  GLADE_HOOKUP_OBJECT (Fenetre, label11288, "label11288");
  GLADE_HOOKUP_OBJECT (Fenetre, image202, "image202");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox332, "hbox332");
  GLADE_HOOKUP_OBJECT (Fenetre, label11289, "label11289");
  GLADE_HOOKUP_OBJECT (Fenetre, image203, "image203");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox333, "hbox333");
  GLADE_HOOKUP_OBJECT (Fenetre, label11290, "label11290");
  GLADE_HOOKUP_OBJECT (Fenetre, image204, "image204");
  GLADE_HOOKUP_OBJECT (Fenetre, frame297, "frame297");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment431, "alignment431");
  GLADE_HOOKUP_OBJECT (Fenetre, image205, "image205");
  GLADE_HOOKUP_OBJECT (Fenetre, label11291, "label11291");
  GLADE_HOOKUP_OBJECT (Fenetre, hbuttonbox7, "hbuttonbox7");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_stra_button_bt1, "cei_stra_button_bt1");
  GLADE_HOOKUP_OBJECT (Fenetre, button165, "button165");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_stra_button_bt3, "cei_stra_button_bt3");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_stra_button_bt4, "cei_stra_button_bt4");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_stra_button_reset, "cei_stra_button_reset");
  GLADE_HOOKUP_OBJECT (Fenetre, image206, "image206");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox117, "vbox117");
  GLADE_HOOKUP_OBJECT (Fenetre, table36, "table36");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox118, "vbox118");
  GLADE_HOOKUP_OBJECT (Fenetre, frame298, "frame298");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment432, "alignment432");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox334, "hbox334");
  GLADE_HOOKUP_OBJECT (Fenetre, label11292, "label11292");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow165, "scrolledwindow165");
  GLADE_HOOKUP_OBJECT (Fenetre, textview124, "textview124");
  GLADE_HOOKUP_OBJECT (Fenetre, label11293, "label11293");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox119, "vbox119");
  GLADE_HOOKUP_OBJECT (Fenetre, frame299, "frame299");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment433, "alignment433");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment434, "alignment434");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox335, "hbox335");
  GLADE_HOOKUP_OBJECT (Fenetre, label11294, "label11294");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow166, "scrolledwindow166");
  GLADE_HOOKUP_OBJECT (Fenetre, textview125, "textview125");
  GLADE_HOOKUP_OBJECT (Fenetre, frame300, "frame300");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment435, "alignment435");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox336, "hbox336");
  GLADE_HOOKUP_OBJECT (Fenetre, entry74, "entry74");
  GLADE_HOOKUP_OBJECT (Fenetre, button169, "button169");
  GLADE_HOOKUP_OBJECT (Fenetre, image207, "image207");
  GLADE_HOOKUP_OBJECT (Fenetre, label11295, "label11295");
  GLADE_HOOKUP_OBJECT (Fenetre, label11296, "label11296");
  GLADE_HOOKUP_OBJECT (Fenetre, frame301, "frame301");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment436, "alignment436");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment437, "alignment437");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox337, "hbox337");
  GLADE_HOOKUP_OBJECT (Fenetre, label11297, "label11297");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow167, "scrolledwindow167");
  GLADE_HOOKUP_OBJECT (Fenetre, textview126, "textview126");
  GLADE_HOOKUP_OBJECT (Fenetre, label11298, "label11298");
  GLADE_HOOKUP_OBJECT (Fenetre, frame302, "frame302");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment438, "alignment438");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox338, "hbox338");
  GLADE_HOOKUP_OBJECT (Fenetre, label11299, "label11299");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow168, "scrolledwindow168");
  GLADE_HOOKUP_OBJECT (Fenetre, textview127, "textview127");
  GLADE_HOOKUP_OBJECT (Fenetre, frame303, "frame303");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment439, "alignment439");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox339, "hbox339");
  GLADE_HOOKUP_OBJECT (Fenetre, entry75, "entry75");
  GLADE_HOOKUP_OBJECT (Fenetre, button170, "button170");
  GLADE_HOOKUP_OBJECT (Fenetre, image208, "image208");
  GLADE_HOOKUP_OBJECT (Fenetre, label11300, "label11300");
  GLADE_HOOKUP_OBJECT (Fenetre, label11301, "label11301");
  GLADE_HOOKUP_OBJECT (Fenetre, frame304, "frame304");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment440, "alignment440");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment441, "alignment441");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox340, "hbox340");
  GLADE_HOOKUP_OBJECT (Fenetre, label11302, "label11302");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow169, "scrolledwindow169");
  GLADE_HOOKUP_OBJECT (Fenetre, textview128, "textview128");
  GLADE_HOOKUP_OBJECT (Fenetre, frame305, "frame305");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment442, "alignment442");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox341, "hbox341");
  GLADE_HOOKUP_OBJECT (Fenetre, entry76, "entry76");
  GLADE_HOOKUP_OBJECT (Fenetre, button171, "button171");
  GLADE_HOOKUP_OBJECT (Fenetre, image209, "image209");
  GLADE_HOOKUP_OBJECT (Fenetre, label11303, "label11303");
  GLADE_HOOKUP_OBJECT (Fenetre, label11304, "label11304");
  GLADE_HOOKUP_OBJECT (Fenetre, frame306, "frame306");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment443, "alignment443");
  GLADE_HOOKUP_OBJECT (Fenetre, drawingarea12, "drawingarea12");
  GLADE_HOOKUP_OBJECT (Fenetre, label11305, "label11305");
  GLADE_HOOKUP_OBJECT (Fenetre, frame307, "frame307");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment444, "alignment444");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment445, "alignment445");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox342, "hbox342");
  GLADE_HOOKUP_OBJECT (Fenetre, label11306, "label11306");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow170, "scrolledwindow170");
  GLADE_HOOKUP_OBJECT (Fenetre, textview129, "textview129");
  GLADE_HOOKUP_OBJECT (Fenetre, frame308, "frame308");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment446, "alignment446");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox343, "hbox343");
  GLADE_HOOKUP_OBJECT (Fenetre, entry77, "entry77");
  GLADE_HOOKUP_OBJECT (Fenetre, button172, "button172");
  GLADE_HOOKUP_OBJECT (Fenetre, image210, "image210");
  GLADE_HOOKUP_OBJECT (Fenetre, label11307, "label11307");
  GLADE_HOOKUP_OBJECT (Fenetre, label11308, "label11308");
  GLADE_HOOKUP_OBJECT (Fenetre, frame309, "frame309");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment447, "alignment447");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox344, "hbox344");
  GLADE_HOOKUP_OBJECT (Fenetre, frame310, "frame310");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment448, "alignment448");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow171, "scrolledwindow171");
  GLADE_HOOKUP_OBJECT (Fenetre, textview130, "textview130");
  GLADE_HOOKUP_OBJECT (Fenetre, label11309, "label11309");
  GLADE_HOOKUP_OBJECT (Fenetre, frame311, "frame311");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment449, "alignment449");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox345, "hbox345");
  GLADE_HOOKUP_OBJECT (Fenetre, entry78, "entry78");
  GLADE_HOOKUP_OBJECT (Fenetre, button173, "button173");
  GLADE_HOOKUP_OBJECT (Fenetre, image211, "image211");
  GLADE_HOOKUP_OBJECT (Fenetre, label11310, "label11310");
  GLADE_HOOKUP_OBJECT (Fenetre, label11311, "label11311");
  GLADE_HOOKUP_OBJECT (Fenetre, frame312, "frame312");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment450, "alignment450");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment451, "alignment451");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox346, "hbox346");
  GLADE_HOOKUP_OBJECT (Fenetre, label11312, "label11312");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow172, "scrolledwindow172");
  GLADE_HOOKUP_OBJECT (Fenetre, textview131, "textview131");
  GLADE_HOOKUP_OBJECT (Fenetre, label11313, "label11313");
  GLADE_HOOKUP_OBJECT (Fenetre, label666, "label666");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox120, "vbox120");
  GLADE_HOOKUP_OBJECT (Fenetre, table37, "table37");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow173, "scrolledwindow173");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport10, "viewport10");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox121, "vbox121");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox347, "hbox347");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox348, "hbox348");
  GLADE_HOOKUP_OBJECT (Fenetre, label11314, "label11314");
  GLADE_HOOKUP_OBJECT (Fenetre, image212, "image212");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox349, "hbox349");
  GLADE_HOOKUP_OBJECT (Fenetre, label11315, "label11315");
  GLADE_HOOKUP_OBJECT (Fenetre, image213, "image213");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox350, "hbox350");
  GLADE_HOOKUP_OBJECT (Fenetre, label11316, "label11316");
  GLADE_HOOKUP_OBJECT (Fenetre, image214, "image214");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox351, "hbox351");
  GLADE_HOOKUP_OBJECT (Fenetre, label11317, "label11317");
  GLADE_HOOKUP_OBJECT (Fenetre, image215, "image215");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox352, "hbox352");
  GLADE_HOOKUP_OBJECT (Fenetre, label11318, "label11318");
  GLADE_HOOKUP_OBJECT (Fenetre, image216, "image216");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox353, "hbox353");
  GLADE_HOOKUP_OBJECT (Fenetre, label11319, "label11319");
  GLADE_HOOKUP_OBJECT (Fenetre, image217, "image217");
  GLADE_HOOKUP_OBJECT (Fenetre, frame313, "frame313");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment452, "alignment452");
  GLADE_HOOKUP_OBJECT (Fenetre, image218, "image218");
  GLADE_HOOKUP_OBJECT (Fenetre, label11320, "label11320");
  GLADE_HOOKUP_OBJECT (Fenetre, hbuttonbox8, "hbuttonbox8");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_sup_button_bt1, "cei_sup_button_bt1");
  GLADE_HOOKUP_OBJECT (Fenetre, cei_sup_button_bt2, "cei_sup_button_bt2");
  GLADE_HOOKUP_OBJECT (Fenetre, button176, "button176");
  GLADE_HOOKUP_OBJECT (Fenetre, button177, "button177");
  GLADE_HOOKUP_OBJECT (Fenetre, button178, "button178");
  GLADE_HOOKUP_OBJECT (Fenetre, image219, "image219");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox122, "vbox122");
  GLADE_HOOKUP_OBJECT (Fenetre, table38, "table38");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox123, "vbox123");
  GLADE_HOOKUP_OBJECT (Fenetre, frame314, "frame314");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment453, "alignment453");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox354, "hbox354");
  GLADE_HOOKUP_OBJECT (Fenetre, label11321, "label11321");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow174, "scrolledwindow174");
  GLADE_HOOKUP_OBJECT (Fenetre, textview132, "textview132");
  GLADE_HOOKUP_OBJECT (Fenetre, label11322, "label11322");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox124, "vbox124");
  GLADE_HOOKUP_OBJECT (Fenetre, frame315, "frame315");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment454, "alignment454");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment455, "alignment455");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox355, "hbox355");
  GLADE_HOOKUP_OBJECT (Fenetre, label11323, "label11323");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow175, "scrolledwindow175");
  GLADE_HOOKUP_OBJECT (Fenetre, textview133, "textview133");
  GLADE_HOOKUP_OBJECT (Fenetre, frame316, "frame316");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment456, "alignment456");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox356, "hbox356");
  GLADE_HOOKUP_OBJECT (Fenetre, entry79, "entry79");
  GLADE_HOOKUP_OBJECT (Fenetre, button179, "button179");
  GLADE_HOOKUP_OBJECT (Fenetre, image220, "image220");
  GLADE_HOOKUP_OBJECT (Fenetre, label11324, "label11324");
  GLADE_HOOKUP_OBJECT (Fenetre, label11325, "label11325");
  GLADE_HOOKUP_OBJECT (Fenetre, frame317, "frame317");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment457, "alignment457");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment458, "alignment458");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox357, "hbox357");
  GLADE_HOOKUP_OBJECT (Fenetre, label11326, "label11326");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow176, "scrolledwindow176");
  GLADE_HOOKUP_OBJECT (Fenetre, textview134, "textview134");
  GLADE_HOOKUP_OBJECT (Fenetre, label11327, "label11327");
  GLADE_HOOKUP_OBJECT (Fenetre, frame318, "frame318");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment459, "alignment459");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox358, "hbox358");
  GLADE_HOOKUP_OBJECT (Fenetre, label11328, "label11328");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow177, "scrolledwindow177");
  GLADE_HOOKUP_OBJECT (Fenetre, textview135, "textview135");
  GLADE_HOOKUP_OBJECT (Fenetre, frame319, "frame319");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment460, "alignment460");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox359, "hbox359");
  GLADE_HOOKUP_OBJECT (Fenetre, entry80, "entry80");
  GLADE_HOOKUP_OBJECT (Fenetre, button180, "button180");
  GLADE_HOOKUP_OBJECT (Fenetre, image221, "image221");
  GLADE_HOOKUP_OBJECT (Fenetre, label11329, "label11329");
  GLADE_HOOKUP_OBJECT (Fenetre, label11330, "label11330");
  GLADE_HOOKUP_OBJECT (Fenetre, frame320, "frame320");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment461, "alignment461");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment462, "alignment462");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox360, "hbox360");
  GLADE_HOOKUP_OBJECT (Fenetre, label11331, "label11331");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow178, "scrolledwindow178");
  GLADE_HOOKUP_OBJECT (Fenetre, textview136, "textview136");
  GLADE_HOOKUP_OBJECT (Fenetre, frame321, "frame321");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment463, "alignment463");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox361, "hbox361");
  GLADE_HOOKUP_OBJECT (Fenetre, entry81, "entry81");
  GLADE_HOOKUP_OBJECT (Fenetre, button181, "button181");
  GLADE_HOOKUP_OBJECT (Fenetre, image222, "image222");
  GLADE_HOOKUP_OBJECT (Fenetre, label11332, "label11332");
  GLADE_HOOKUP_OBJECT (Fenetre, label11333, "label11333");
  GLADE_HOOKUP_OBJECT (Fenetre, frame322, "frame322");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment464, "alignment464");
  GLADE_HOOKUP_OBJECT (Fenetre, drawingarea13, "drawingarea13");
  GLADE_HOOKUP_OBJECT (Fenetre, label11334, "label11334");
  GLADE_HOOKUP_OBJECT (Fenetre, frame323, "frame323");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment465, "alignment465");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment466, "alignment466");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox362, "hbox362");
  GLADE_HOOKUP_OBJECT (Fenetre, label11335, "label11335");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow179, "scrolledwindow179");
  GLADE_HOOKUP_OBJECT (Fenetre, textview137, "textview137");
  GLADE_HOOKUP_OBJECT (Fenetre, frame324, "frame324");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment467, "alignment467");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox363, "hbox363");
  GLADE_HOOKUP_OBJECT (Fenetre, entry82, "entry82");
  GLADE_HOOKUP_OBJECT (Fenetre, button182, "button182");
  GLADE_HOOKUP_OBJECT (Fenetre, image223, "image223");
  GLADE_HOOKUP_OBJECT (Fenetre, label11336, "label11336");
  GLADE_HOOKUP_OBJECT (Fenetre, label11337, "label11337");
  GLADE_HOOKUP_OBJECT (Fenetre, frame325, "frame325");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment468, "alignment468");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox364, "hbox364");
  GLADE_HOOKUP_OBJECT (Fenetre, frame326, "frame326");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment469, "alignment469");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow180, "scrolledwindow180");
  GLADE_HOOKUP_OBJECT (Fenetre, textview138, "textview138");
  GLADE_HOOKUP_OBJECT (Fenetre, label11338, "label11338");
  GLADE_HOOKUP_OBJECT (Fenetre, frame327, "frame327");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment470, "alignment470");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox365, "hbox365");
  GLADE_HOOKUP_OBJECT (Fenetre, entry83, "entry83");
  GLADE_HOOKUP_OBJECT (Fenetre, button183, "button183");
  GLADE_HOOKUP_OBJECT (Fenetre, image224, "image224");
  GLADE_HOOKUP_OBJECT (Fenetre, label11339, "label11339");
  GLADE_HOOKUP_OBJECT (Fenetre, label11340, "label11340");
  GLADE_HOOKUP_OBJECT (Fenetre, frame328, "frame328");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment471, "alignment471");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment472, "alignment472");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox366, "hbox366");
  GLADE_HOOKUP_OBJECT (Fenetre, label11341, "label11341");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow181, "scrolledwindow181");
  GLADE_HOOKUP_OBJECT (Fenetre, textview139, "textview139");
  GLADE_HOOKUP_OBJECT (Fenetre, label11342, "label11342");
  GLADE_HOOKUP_OBJECT (Fenetre, label20, "label20");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox125, "vbox125");
  GLADE_HOOKUP_OBJECT (Fenetre, table39, "table39");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow182, "scrolledwindow182");
  GLADE_HOOKUP_OBJECT (Fenetre, viewport11, "viewport11");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox126, "vbox126");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox367, "hbox367");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox368, "hbox368");
  GLADE_HOOKUP_OBJECT (Fenetre, label11343, "label11343");
  GLADE_HOOKUP_OBJECT (Fenetre, image225, "image225");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox369, "hbox369");
  GLADE_HOOKUP_OBJECT (Fenetre, label11344, "label11344");
  GLADE_HOOKUP_OBJECT (Fenetre, image226, "image226");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox370, "hbox370");
  GLADE_HOOKUP_OBJECT (Fenetre, label11345, "label11345");
  GLADE_HOOKUP_OBJECT (Fenetre, image227, "image227");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox371, "hbox371");
  GLADE_HOOKUP_OBJECT (Fenetre, label11346, "label11346");
  GLADE_HOOKUP_OBJECT (Fenetre, image228, "image228");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox372, "hbox372");
  GLADE_HOOKUP_OBJECT (Fenetre, label11347, "label11347");
  GLADE_HOOKUP_OBJECT (Fenetre, image229, "image229");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox373, "hbox373");
  GLADE_HOOKUP_OBJECT (Fenetre, label11348, "label11348");
  GLADE_HOOKUP_OBJECT (Fenetre, image230, "image230");
  GLADE_HOOKUP_OBJECT (Fenetre, frame329, "frame329");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment473, "alignment473");
  GLADE_HOOKUP_OBJECT (Fenetre, image231, "image231");
  GLADE_HOOKUP_OBJECT (Fenetre, label11349, "label11349");
  GLADE_HOOKUP_OBJECT (Fenetre, hbuttonbox9, "hbuttonbox9");
  GLADE_HOOKUP_OBJECT (Fenetre, button184, "button184");
  GLADE_HOOKUP_OBJECT (Fenetre, button185, "button185");
  GLADE_HOOKUP_OBJECT (Fenetre, button186, "button186");
  GLADE_HOOKUP_OBJECT (Fenetre, button187, "button187");
  GLADE_HOOKUP_OBJECT (Fenetre, button188, "button188");
  GLADE_HOOKUP_OBJECT (Fenetre, image232, "image232");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox127, "vbox127");
  GLADE_HOOKUP_OBJECT (Fenetre, table40, "table40");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox128, "vbox128");
  GLADE_HOOKUP_OBJECT (Fenetre, frame330, "frame330");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment474, "alignment474");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox374, "hbox374");
  GLADE_HOOKUP_OBJECT (Fenetre, label11350, "label11350");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow183, "scrolledwindow183");
  GLADE_HOOKUP_OBJECT (Fenetre, textview140, "textview140");
  GLADE_HOOKUP_OBJECT (Fenetre, label11351, "label11351");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox129, "vbox129");
  GLADE_HOOKUP_OBJECT (Fenetre, frame331, "frame331");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment475, "alignment475");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment476, "alignment476");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox375, "hbox375");
  GLADE_HOOKUP_OBJECT (Fenetre, label11352, "label11352");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow184, "scrolledwindow184");
  GLADE_HOOKUP_OBJECT (Fenetre, textview141, "textview141");
  GLADE_HOOKUP_OBJECT (Fenetre, frame332, "frame332");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment477, "alignment477");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox376, "hbox376");
  GLADE_HOOKUP_OBJECT (Fenetre, entry84, "entry84");
  GLADE_HOOKUP_OBJECT (Fenetre, button189, "button189");
  GLADE_HOOKUP_OBJECT (Fenetre, image233, "image233");
  GLADE_HOOKUP_OBJECT (Fenetre, label11353, "label11353");
  GLADE_HOOKUP_OBJECT (Fenetre, label11354, "label11354");
  GLADE_HOOKUP_OBJECT (Fenetre, frame333, "frame333");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment478, "alignment478");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment479, "alignment479");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox377, "hbox377");
  GLADE_HOOKUP_OBJECT (Fenetre, label11355, "label11355");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow185, "scrolledwindow185");
  GLADE_HOOKUP_OBJECT (Fenetre, textview142, "textview142");
  GLADE_HOOKUP_OBJECT (Fenetre, label11356, "label11356");
  GLADE_HOOKUP_OBJECT (Fenetre, frame334, "frame334");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment480, "alignment480");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox378, "hbox378");
  GLADE_HOOKUP_OBJECT (Fenetre, label11357, "label11357");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow186, "scrolledwindow186");
  GLADE_HOOKUP_OBJECT (Fenetre, textview143, "textview143");
  GLADE_HOOKUP_OBJECT (Fenetre, frame335, "frame335");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment481, "alignment481");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox379, "hbox379");
  GLADE_HOOKUP_OBJECT (Fenetre, entry85, "entry85");
  GLADE_HOOKUP_OBJECT (Fenetre, button190, "button190");
  GLADE_HOOKUP_OBJECT (Fenetre, image234, "image234");
  GLADE_HOOKUP_OBJECT (Fenetre, label11358, "label11358");
  GLADE_HOOKUP_OBJECT (Fenetre, label11359, "label11359");
  GLADE_HOOKUP_OBJECT (Fenetre, frame336, "frame336");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment482, "alignment482");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment483, "alignment483");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox380, "hbox380");
  GLADE_HOOKUP_OBJECT (Fenetre, label11360, "label11360");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow187, "scrolledwindow187");
  GLADE_HOOKUP_OBJECT (Fenetre, textview144, "textview144");
  GLADE_HOOKUP_OBJECT (Fenetre, frame337, "frame337");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment484, "alignment484");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox381, "hbox381");
  GLADE_HOOKUP_OBJECT (Fenetre, entry86, "entry86");
  GLADE_HOOKUP_OBJECT (Fenetre, button191, "button191");
  GLADE_HOOKUP_OBJECT (Fenetre, image235, "image235");
  GLADE_HOOKUP_OBJECT (Fenetre, label11361, "label11361");
  GLADE_HOOKUP_OBJECT (Fenetre, label11362, "label11362");
  GLADE_HOOKUP_OBJECT (Fenetre, frame338, "frame338");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment485, "alignment485");
  GLADE_HOOKUP_OBJECT (Fenetre, drawingarea14, "drawingarea14");
  GLADE_HOOKUP_OBJECT (Fenetre, label11363, "label11363");
  GLADE_HOOKUP_OBJECT (Fenetre, frame339, "frame339");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment486, "alignment486");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment487, "alignment487");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox382, "hbox382");
  GLADE_HOOKUP_OBJECT (Fenetre, label11364, "label11364");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow188, "scrolledwindow188");
  GLADE_HOOKUP_OBJECT (Fenetre, textview145, "textview145");
  GLADE_HOOKUP_OBJECT (Fenetre, frame340, "frame340");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment488, "alignment488");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox383, "hbox383");
  GLADE_HOOKUP_OBJECT (Fenetre, entry87, "entry87");
  GLADE_HOOKUP_OBJECT (Fenetre, button192, "button192");
  GLADE_HOOKUP_OBJECT (Fenetre, image236, "image236");
  GLADE_HOOKUP_OBJECT (Fenetre, label11365, "label11365");
  GLADE_HOOKUP_OBJECT (Fenetre, label11366, "label11366");
  GLADE_HOOKUP_OBJECT (Fenetre, frame341, "frame341");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment489, "alignment489");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox384, "hbox384");
  GLADE_HOOKUP_OBJECT (Fenetre, frame342, "frame342");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment490, "alignment490");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow189, "scrolledwindow189");
  GLADE_HOOKUP_OBJECT (Fenetre, textview146, "textview146");
  GLADE_HOOKUP_OBJECT (Fenetre, label11367, "label11367");
  GLADE_HOOKUP_OBJECT (Fenetre, frame343, "frame343");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment491, "alignment491");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox385, "hbox385");
  GLADE_HOOKUP_OBJECT (Fenetre, entry88, "entry88");
  GLADE_HOOKUP_OBJECT (Fenetre, button193, "button193");
  GLADE_HOOKUP_OBJECT (Fenetre, image237, "image237");
  GLADE_HOOKUP_OBJECT (Fenetre, label11368, "label11368");
  GLADE_HOOKUP_OBJECT (Fenetre, label11369, "label11369");
  GLADE_HOOKUP_OBJECT (Fenetre, frame344, "frame344");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment492, "alignment492");
  GLADE_HOOKUP_OBJECT (Fenetre, alignment493, "alignment493");
  GLADE_HOOKUP_OBJECT (Fenetre, hbox386, "hbox386");
  GLADE_HOOKUP_OBJECT (Fenetre, label11370, "label11370");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow190, "scrolledwindow190");
  GLADE_HOOKUP_OBJECT (Fenetre, textview147, "textview147");
  GLADE_HOOKUP_OBJECT (Fenetre, label11371, "label11371");
  GLADE_HOOKUP_OBJECT (Fenetre, label53, "label53");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox10, "vbox10");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox22, "vbox22");
  GLADE_HOOKUP_OBJECT (Fenetre, toolbar7, "toolbar7");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem31, "toolitem31");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_cb_cb1, "uart_cb_cb1");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem33, "toolitem33");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_button_clear1, "uart_button_clear1");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem34, "toolitem34");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_chk_keep1, "uart_chk_keep1");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow9, "scrolledwindow9");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_zt_uart1, "uart_zt_uart1");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox23, "vbox23");
  GLADE_HOOKUP_OBJECT (Fenetre, toolbar8, "toolbar8");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem38, "toolitem38");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_cb_cb2, "uart_cb_cb2");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem39, "toolitem39");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_button_clear2, "uart_button_clear2");
  GLADE_HOOKUP_OBJECT (Fenetre, toolitem40, "toolitem40");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_chk_keep2, "uart_chk_keep2");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow10, "scrolledwindow10");
  GLADE_HOOKUP_OBJECT (Fenetre, uart_zt_uart2, "uart_zt_uart2");
  GLADE_HOOKUP_OBJECT (Fenetre, vbox24, "vbox24");
  GLADE_HOOKUP_OBJECT (Fenetre, label28, "label28");
  GLADE_HOOKUP_OBJECT (Fenetre, scrolledwindow153, "scrolledwindow153");
  GLADE_HOOKUP_OBJECT (Fenetre, terminal_zt_user, "terminal_zt_user");

  gtk_window_add_accel_group (GTK_WINDOW (Fenetre), accel_group);

  return Fenetre;
}

