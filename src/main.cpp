#include "main.h"

#define PORT_MOTOR_FL 10        /* Front left.*/
#define PORT_MOTOR_BL 9         /* Back left.*/
#define PORT_MOTOR_FR 20        /* Front right. */
#define PORT_MOTOR_BR 19        /* Back right.*/
#define PORT_MOTOR_INTAKE 8
#define PORT_MOTOR_LIFT 2
#define PORT_SENSOR_ROT 3        /* Rotation sensor. */

#define MOTOR_MAX  127
#define GREEN_RPM 200
#define BLUE_RPM 600
#define RED_RPM 100

pros::Controller ctrler(pros::E_CONTROLLER_MASTER);
pros::Motor 
    left_front(PORT_MOTOR_FL,pros::motor_gearset_e_t::E_MOTOR_GEAR_600, 1),
    left_back(PORT_MOTOR_BL, pros::motor_gearset_e_t::E_MOTOR_GEAR_600, 1),
    right_front(PORT_MOTOR_FR, pros::motor_gearset_e_t::E_MOTOR_GEAR_600, 0),
    right_back(PORT_MOTOR_BR, pros::motor_gearset_e_t::E_MOTOR_GEAR_600, 0),
    lift(PORT_MOTOR_LIFT, pros::motor_gearset_e_t::E_MOTOR_GEAR_100, 1),
    intake(PORT_MOTOR_INTAKE, pros::motor_gearset_e_t::E_MOTOR_GEAR_200, 0);
pros::Motor_Group
    group_left_drive ({left_front, left_back}),
    group_right_drive ({right_front, right_back});

/* Center button callback. */
void on_center_button() {
    static bool pressed = false;
    pressed = !pressed;
    if (pressed) {
        pros::lcd::set_text(2, "hello  there");
    } else {
        pros::lcd::clear_line(2);
    }
}

/**
 * Runs initialization code. This occurs as soon as the program is started.
 *
 * All other competition modes are blocked by initialize; it is recommended
 * to keep execution time for this mode under a few seconds.
 */
void initialize() {
    pros::lcd::initialize();
    pros::lcd::set_text(1, "balls :3");
    group_left_drive.set_brake_modes(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
    group_right_drive.set_brake_modes(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
    lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_COAST);
}

/**
 * Runs while the robot is in the disabled state of Field Management System or
 * the VEX Competition Switch, following either autonomous or opcontrol. When
 * the robot is enabled, this task will exit.
 */
void disabled() {}

/**
 * Runs after initialize(), and before autonomous when connected to the Field
 * Management System or the VEX Competition Switch. This is intended for
 * competition-specific initialization routines, such as an autonomous selector
 * on the LCD.
 *
 * This task will exit when the robot is enabled and autonomous or opcontrol
 * starts.
 */
void competition_initialize() {
    pros::lcd::set_text(2, "Competition Init");
}

/**
 * Runs the user autonomous code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the autonomous
 * mode. Alternatively, this function may be called in initialize or opcontrol
 * for non-competition testing purposes.
 *
 * If the robot is disabled or communications is lost, the autonomous task
 * will be stopped. Re-enabling the robot will restart the task, not re-start it
 * from where it left off.
 */
void autonomous() {
    /* Spin.*/
    while (1) {
        group_right_drive.move(MOTOR_MAX);
        group_left_drive.move(-MOTOR_MAX);
    }
}

/**
 * Runs the operator control code. This function will be started in its own task
 * with the default priority and stack size whenever the robot is enabled via
 * the Field Management System or the VEX Competition Switch in the operator
 * control mode.
 *
 * If no competition control is connected, this function will run immediately
 * following initialize().
 *
 * If the robot is disabled or communications is lost, the
 * operator control task will be stopped. Re-enabling the robot will restart the
 * task, not resume it from where it left off.
 */
void opcontrol() {
    while (1) {
        int
            ljoy_val = ctrler.get_analog(ANALOG_LEFT_Y),    /* Left joystick value.*/
            rjoy_val = ctrler.get_analog(ANALOG_RIGHT_Y),    /* Right joystick value.*/
            lunubbin_val = ctrler.get_digital(DIGITAL_L1),  /* Left upper nubbin value.*/
            llnubbin_val = ctrler.get_digital(DIGITAL_L2),  /* Left lower nubbin value.*/
            runubbin_val = ctrler.get_digital(DIGITAL_R1),  /* Right upper nubbin value.*/
            rlnubbin_val = ctrler.get_digital(DIGITAL_R2),  /* Right lower nubbin value.*/
            ubtn_val = ctrler.get_digital(DIGITAL_UP),      /* Up arrow button value.*/
            dbtn_val = ctrler.get_digital(DIGITAL_DOWN);    /* Down arrow button value.*/

        if (!ljoy_val) { group_left_drive.brake(); }
        if (!rjoy_val) { group_right_drive.brake(); }

        group_left_drive.move(lunubbin_val ? MOTOR_MAX : (llnubbin_val ? -MOTOR_MAX : ljoy_val));
        group_right_drive.move(lunubbin_val ? MOTOR_MAX : (llnubbin_val ? -MOTOR_MAX : rjoy_val));

        if (ubtn_val) {
            lift.move_velocity(RED_RPM);
        } else if (dbtn_val) {
            lift.move_velocity(-RED_RPM);
        } else {
            lift.brake();
        }
        
        if (runubbin_val) {
            intake.move_velocity(GREEN_RPM);
        } else if (rlnubbin_val) {
            intake.move_velocity(-GREEN_RPM);
        } else {
            intake.brake();
        }

        pros::delay(20);
    }
}
