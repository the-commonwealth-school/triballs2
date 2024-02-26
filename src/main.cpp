#include "main.h"


#define PORT_MOTOR_FL 10        /* Front left.*/
#define PORT_MOTOR_BL 9         /* Back left.*/
#define PORT_MOTOR_FR 20        /* Front right. */
#define PORT_MOTOR_BR 19        /* Back right.*/
#define PORT_MOTOR_INTAKE 8
#define PORT_MOTOR_LIFT 2


#define MOTOR_MAX  127
#define GREEN_RPM 200
#define BLUE_RPM 600
#define RED_RPM 100


pros::Controller ctrler(pros::E_CONTROLLER_MASTER);
pros::Motor left_front(PORT_MOTOR_FL,pros::motor_gearset_e_t::E_MOTOR_GEAR_600, false);
pros::Motor left_back(PORT_MOTOR_BL, pros::motor_gearset_e_t::E_MOTOR_GEAR_600, false);
pros::Motor right_front(PORT_MOTOR_FR, pros::motor_gearset_e_t::E_MOTOR_GEAR_600, true);
pros::Motor right_back(PORT_MOTOR_BR, pros::motor_gearset_e_t::E_MOTOR_GEAR_600, true);
pros::Motor lift(PORT_MOTOR_LIFT, pros::motor_gearset_e_t::E_MOTOR_GEAR_100, false);
pros::Motor intake(PORT_MOTOR_INTAKE, pros::motor_gearset_e_t::E_MOTOR_GEAR_200, false);
pros::Motor_Group group_left_drive ({left_front, left_back});
pros::Motor_Group group_right_drive ({right_front, right_back});

/**
 * A callback function for LLEMU's center button.
 *
 * When this callback is fired, it will toggle line 2 of the LCD text between
 * "I was pressed!" and nothing.
 */
void on_center_button() {
    static bool pressed = false;
    pressed = !pressed;
    if (pressed) {
        pros::lcd::set_text(2, "( ͡° ͜ʖ ͡°)");
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
    pros::lcd::set_text(1, "balls");

    pros::lcd::register_btn1_cb(on_center_button);
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
    group_left_drive.set_brake_modes(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
    group_right_drive.set_brake_modes(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_HOLD);
    lift.set_brake_mode(pros::motor_brake_mode_e_t::E_MOTOR_BRAKE_COAST);
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
void autonomous() {}

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

    while (true) {
        int ljoy_val = ctrler.get_analog(ANALOG_LEFT_Y);    /* Left joystick value.*/
        int rjoy_val = ctrler.get_analog(ANALOG_LEFT_Y);    /* Right joystick value.*/

        int lunubbin_val = ctrler.get_digital(DIGITAL_L1);  /* Left upper nubbin value.*/
        int llnubbin_val = ctrler.get_digital(DIGITAL_L2);  /* Left lower nubbin value.*/

        int runubbin_val = ctrler.get_digital(DIGITAL_R1);  /* Right upper nubbin value.*/
        int rlnubbin_val = ctrler.get_digital(DIGITAL_R2);  /* Right lower nubbin value.*/

        int ubtn_val = ctrler.get_digital(DIGITAL_UP);      /* Up arrow button value.*/
        int dbtn_val = ctrler.get_digital(DIGITAL_DOWN);    /* Down arrow button value.*/

        group_left_drive.move(lunubbin_val ? 127 : (llnubbin_val ? -127 : ljoy_val));
        group_right_drive.move(lunubbin_val ? 127 : (llnubbin_val ? -127 : rjoy_val));

        if(ljoy_val == 0) group_left_drive.brake();
        if(rjoy_val == 0) group_right_drive.brake();

        if(ubtn_val) {
            lift.move_velocity(100);
        } else if (dbtn_val) {
            lift.move_velocity(-100);
        } else if (runubbin_val) {
            intake.move_velocity(200);
        } else if (rlnubbin_val) {
            intake.move_velocity(-200);
        }

        pros::delay(20);
    }
}
