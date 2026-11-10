
import java.sql.*;
import java.util.Scanner;

public class PQ29 {

    static final String URL =
        "jdbc:mysql://localhost:3306/StudentDB";

    static final String USER = "root";
    static final String PASSWORD = "your_password";

    static Connection con;
    static Scanner sc = new Scanner(System.in);

    // Database Connection
    static void connect() {
        try {
            con = DriverManager.getConnection(
                URL, USER, PASSWORD);

            System.out.println("Database Connected Successfully!");

        } catch (Exception e) {
            System.out.println("Connection Error: " + e.getMessage());
        }
    }


    // ==========================================
    // PART A: ADD, SEARCH AND MODIFY STUDENT
    // USING JDBC AND PREPARED STATEMENT
    // ==========================================

    // A1. Add Student Record
    static void addStudent() {
        try {
            System.out.print("Enter Student ID: ");
            int id = sc.nextInt();
            sc.nextLine();

            System.out.print("Enter Student Name: ");
            String name = sc.nextLine();

            System.out.print("Enter Course: ");
            String course = sc.nextLine();

            System.out.print("Enter Marks: ");
            int marks = sc.nextInt();

            String query =
                "INSERT INTO Students VALUES (?, ?, ?, ?)";

            PreparedStatement ps = con.prepareStatement(query);

            ps.setInt(1, id);
            ps.setString(2, name);
            ps.setString(3, course);
            ps.setInt(4, marks);

            ps.executeUpdate();

            System.out.println("Student Added Successfully!");

            ps.close();

        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
    }


    // A2. Search Student Record
    static void searchStudent() {
        try {
            System.out.print("Enter Student ID: ");
            int id = sc.nextInt();

            String query =
                "SELECT * FROM Students WHERE ID = ?";

            PreparedStatement ps = con.prepareStatement(query);

            ps.setInt(1, id);

            ResultSet rs = ps.executeQuery();

            if (rs.next()) {
                System.out.println("\nStudent Details:");
                System.out.println("ID: " + rs.getInt("ID"));
                System.out.println("Name: " + rs.getString("Name"));
                System.out.println("Course: " + rs.getString("Course"));
                System.out.println("Marks: " + rs.getInt("Marks"));
            } else {
                System.out.println("Student Not Found!");
            }

            rs.close();
            ps.close();

        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
    }


    // A3. Modify Student Record
    static void modifyStudent() {
        try {
            System.out.print("Enter Student ID: ");
            int id = sc.nextInt();
            sc.nextLine();

            System.out.print("Enter New Name: ");
            String name = sc.nextLine();

            System.out.print("Enter New Course: ");
            String course = sc.nextLine();

            System.out.print("Enter New Marks: ");
            int marks = sc.nextInt();

            String query =
                "UPDATE Students SET Name=?, Course=?, Marks=? WHERE ID=?";

            PreparedStatement ps = con.prepareStatement(query);

            ps.setString(1, name);
            ps.setString(2, course);
            ps.setInt(3, marks);
            ps.setInt(4, id);

            int result = ps.executeUpdate();

            if (result > 0) {
                System.out.println("Student Updated Successfully!");
            } else {
                System.out.println("Student Not Found!");
            }

            ps.close();

        } catch (Exception e) {
            System.out.println("Error: " + e.getMessage());
        }
    }


    // ==========================================
    // PART B: CALL BACKEND STORED PROCEDURE
    // USING STUDENT ID
    // ==========================================

    static void callProcedure() {
        try {
            System.out.print("Enter Student ID: ");
            int id = sc.nextInt();

            CallableStatement cs =
                con.prepareCall("{CALL GetStudent(?)}");

            cs.setInt(1, id);

            ResultSet rs = cs.executeQuery();

            if (rs.next()) {
                System.out.println("\nStudent Details From Procedure:");
                System.out.println("ID: " + rs.getInt("ID"));
                System.out.println("Name: " + rs.getString("Name"));
                System.out.println("Course: " + rs.getString("Course"));
                System.out.println("Marks: " + rs.getInt("Marks"));
            } else {
                System.out.println("Student Not Found!");
            }

            rs.close();
            cs.close();

        } catch (Exception e) {
            System.out.println("Procedure Error: " + e.getMessage());
        }
    }


    // ==========================================
    // MAIN METHOD
    // ==========================================

    public static void main(String[] args) {

        connect();

        if (con == null) {
            return;
        }

        while (true) {

            System.out.println("\n===== STUDENT MANAGEMENT SYSTEM =====");

            System.out.println("\n----- PART A -----");
            System.out.println("1. Add Student");
            System.out.println("2. Search Student");
            System.out.println("3. Modify Student");

            System.out.println("\n----- PART B -----");
            System.out.println("4. Call Stored Procedure");

            System.out.println("\n5. Exit");

            System.out.print("\nEnter Your Choice: ");

            int choice = sc.nextInt();

            switch (choice) {

                case 1:
                    addStudent();
                    break;

                case 2:
                    searchStudent();
                    break;

                case 3:
                    modifyStudent();
                    break;

                case 4:
                    callProcedure();
                    break;

                case 5:
                    System.out.println("Thank You!");
                    try {
                        con.close();
                    } catch (Exception e) {
                        System.out.println(e);
                    }
                    return;

                default:
                    System.out.println("Invalid Choice!");
            }
        }
    }
}







