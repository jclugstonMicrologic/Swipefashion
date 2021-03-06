/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.4.0-dev at Thu Mar 21 15:14:13 2019. */

#include "tloadpckts.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t STANDARD_PACKET_fields[5] = {
    PB_FIELD(  1, UINT32  , REQUIRED, STATIC  , FIRST, STANDARD_PACKET, type, type, 0),
    PB_FIELD(  2, UINT32  , REQUIRED, STATIC  , OTHER, STANDARD_PACKET, gwId, type, 0),
    PB_FIELD(  3, UINT32  , REQUIRED, STATIC  , OTHER, STANDARD_PACKET, timeStamp, gwId, 0),
    PB_FIELD(  4, UINT32  , REQUIRED, STATIC  , OTHER, STANDARD_PACKET, messageId, timeStamp, 0),
    PB_LAST_FIELD
};

const pb_field_t STATUS_PACKET_fields[3] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, STATUS_PACKET, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, UINT32  , REQUIRED, STATIC  , OTHER, STATUS_PACKET, status, std, 0),
    PB_LAST_FIELD
};

const pb_field_t SENSOR_DATA_fields[11] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, SENSOR_DATA, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, outage, std, 0),
    PB_FIELD(  3, FLOAT   , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, flowRate, outage, 0),
    PB_FIELD(  4, FLOAT   , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, vbat, flowRate, 0),
    PB_FIELD(  5, FLOAT   , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, temp, vbat, 0),
    PB_FIELD(  6, FLOAT   , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, density, temp, 0),
    PB_FIELD(  7, FLOAT   , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, volume, density, 0),
    PB_FIELD(  8, UINT32  , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, loadId, volume, 0),
    PB_FIELD(  9, UINT32  , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, smplNbr, loadId, 0),
    PB_FIELD( 10, UINT32  , OPTIONAL, STATIC  , OTHER, SENSOR_DATA, spotNbr, smplNbr, 0),
    PB_LAST_FIELD
};

const pb_field_t COMPLETED_DATA_fields[5] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, COMPLETED_DATA, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, UINT32  , OPTIONAL, STATIC  , OTHER, COMPLETED_DATA, spotNbr, std, 0),
    PB_FIELD(  3, BOOL    , OPTIONAL, STATIC  , OTHER, COMPLETED_DATA, completed, spotNbr, 0),
    PB_FIELD(  4, UINT32  , OPTIONAL, STATIC  , OTHER, COMPLETED_DATA, loadPlanId, completed, 0),
    PB_LAST_FIELD
};

const pb_field_t GPS_DATA_fields[7] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, GPS_DATA, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, GPS_DATA, lat, std, 0),
    PB_FIELD(  3, FLOAT   , OPTIONAL, STATIC  , OTHER, GPS_DATA, lon, lat, 0),
    PB_FIELD(  4, FLOAT   , OPTIONAL, STATIC  , OTHER, GPS_DATA, alt, lon, 0),
    PB_FIELD(  5, FLOAT   , OPTIONAL, STATIC  , OTHER, GPS_DATA, speed, alt, 0),
    PB_FIELD(  6, BOOL    , OPTIONAL, STATIC  , OTHER, GPS_DATA, status, speed, 0),
    PB_LAST_FIELD
};

const pb_field_t REGISTER_GTW_CMD_fields[2] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, REGISTER_GTW_CMD, std, std, &STANDARD_PACKET_fields),
    PB_LAST_FIELD
};

const pb_field_t ACCELEROMETER_DATA_fields[5] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, ACCELEROMETER_DATA, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, INT32   , OPTIONAL, STATIC  , OTHER, ACCELEROMETER_DATA, accelX, std, 0),
    PB_FIELD(  3, INT32   , OPTIONAL, STATIC  , OTHER, ACCELEROMETER_DATA, accelY, accelX, 0),
    PB_FIELD(  4, INT32   , OPTIONAL, STATIC  , OTHER, ACCELEROMETER_DATA, accelZ, accelY, 0),
    PB_LAST_FIELD
};

const pb_field_t METER_TICKET_DATA_fields[27] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, METER_TICKET_DATA, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, reqSeqnbrTarget, std, 0),
    PB_FIELD(  3, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, yymmdd, reqSeqnbrTarget, 0),
    PB_FIELD(  4, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, hhmm, yymmdd, 0),
    PB_FIELD(  5, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, ticketTruckCode, hhmm, 0),
    PB_FIELD(  6, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, ticketWellCode, ticketTruckCode, 0),
    PB_FIELD(  7, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, weighBill, ticketWellCode, 0),
    PB_FIELD(  8, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, gccOil, weighBill, 0),
    PB_FIELD(  9, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, gccWater, gccOil, 0),
    PB_FIELD( 10, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, flowWeightedAvgDegC, gccWater, 0),
    PB_FIELD( 11, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, perWaterCut, flowWeightedAvgDegC, 0),
    PB_FIELD( 12, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, loadOilM3, perWaterCut, 0),
    PB_FIELD( 13, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, loadWaterM3, loadOilM3, 0),
    PB_FIELD( 14, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, normLoadM3, loadWaterM3, 0),
    PB_FIELD( 15, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, flowWeightedAvgkGM3, normLoadM3, 0),
    PB_FIELD( 16, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, autoCompOil, flowWeightedAvgkGM3, 0),
    PB_FIELD( 17, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, autoCompWater, autoCompOil, 0),
    PB_FIELD( 18, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, autoCompPer, autoCompWater, 0),
    PB_FIELD( 19, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, flowMins, autoCompPer, 0),
    PB_FIELD( 20, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, shrinkage, flowMins, 0),
    PB_FIELD( 21, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, dryTankM3, shrinkage, 0),
    PB_FIELD( 22, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, wetTankM3, dryTankM3, 0),
    PB_FIELD( 23, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, loadKg, wetTankM3, 0),
    PB_FIELD( 24, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, futureTicketPar1, loadKg, 0),
    PB_FIELD( 25, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, futureTicketPar2, futureTicketPar1, 0),
    PB_FIELD( 26, FLOAT   , OPTIONAL, STATIC  , OTHER, METER_TICKET_DATA, futureTicketPar3, futureTicketPar2, 0),
    PB_LAST_FIELD
};

const pb_field_t LEVEL_DATA_fields[3] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, LEVEL_DATA, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, LEVEL_DATA, level, std, 0),
    PB_LAST_FIELD
};

const pb_field_t LOAD_PLAN_fields[5] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, LOAD_PLAN, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, LOAD_PLAN, shutoffVolume, std, 0),
    PB_FIELD(  3, UINT32  , OPTIONAL, STATIC  , OTHER, LOAD_PLAN, id, shutoffVolume, 0),
    PB_FIELD(  4, FLOAT   , REPEATED, STATIC  , OTHER, LOAD_PLAN, coeff, id, 0),
    PB_LAST_FIELD
};

const pb_field_t FW_UPDATE_VERSION_fields[3] = {
    PB_FIELD(  1, MESSAGE , REQUIRED, STATIC  , FIRST, FW_UPDATE_VERSION, std, std, &STANDARD_PACKET_fields),
    PB_FIELD(  2, FLOAT   , OPTIONAL, STATIC  , OTHER, FW_UPDATE_VERSION, fwVersion, std, 0),
    PB_LAST_FIELD
};




/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(STATUS_PACKET, std) < 65536 && pb_membersize(SENSOR_DATA, std) < 65536 && pb_membersize(COMPLETED_DATA, std) < 65536 && pb_membersize(GPS_DATA, std) < 65536 && pb_membersize(REGISTER_GTW_CMD, std) < 65536 && pb_membersize(ACCELEROMETER_DATA, std) < 65536 && pb_membersize(METER_TICKET_DATA, std) < 65536 && pb_membersize(LEVEL_DATA, std) < 65536 && pb_membersize(LOAD_PLAN, std) < 65536 && pb_membersize(FW_UPDATE_VERSION, std) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_STANDARD_PACKET_STATUS_PACKET_SENSOR_DATA_COMPLETED_DATA_GPS_DATA_REGISTER_GTW_CMD_ACCELEROMETER_DATA_METER_TICKET_DATA_LEVEL_DATA_LOAD_PLAN_FW_UPDATE_VERSION)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(STATUS_PACKET, std) < 256 && pb_membersize(SENSOR_DATA, std) < 256 && pb_membersize(COMPLETED_DATA, std) < 256 && pb_membersize(GPS_DATA, std) < 256 && pb_membersize(REGISTER_GTW_CMD, std) < 256 && pb_membersize(ACCELEROMETER_DATA, std) < 256 && pb_membersize(METER_TICKET_DATA, std) < 256 && pb_membersize(LEVEL_DATA, std) < 256 && pb_membersize(LOAD_PLAN, std) < 256 && pb_membersize(FW_UPDATE_VERSION, std) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_STANDARD_PACKET_STATUS_PACKET_SENSOR_DATA_COMPLETED_DATA_GPS_DATA_REGISTER_GTW_CMD_ACCELEROMETER_DATA_METER_TICKET_DATA_LEVEL_DATA_LOAD_PLAN_FW_UPDATE_VERSION)
#endif


/* @@protoc_insertion_point(eof) */
