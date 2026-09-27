#include <iostream>
#include <vector>
#include <string>
#include <limits>

using namespace std;

struct Task
{
    string description;
    bool completed;
};

vector<Task> tasks;

// Add a new task
void addTask()
{
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    string taskDescription;

    cout << "\nEnter task: ";
    getline(cin, taskDescription);

    if (taskDescription.empty())
    {
        cout << "Task cannot be empty!\n";
        return;
    }

    tasks.push_back({taskDescription, false});

    cout << "Task added successfully!\n";
}

// Display all tasks
void viewTasks()
{
    if (tasks.empty())
    {
        cout << "\nNo tasks available.\n";
        return;
    }

    cout << "\n========== TO-DO LIST ==========\n";

    for (size_t i = 0; i < tasks.size(); i++)
    {
        cout << i + 1 << ". ";

        if (tasks[i].completed)
        {
            cout << "[Completed] ";
        }
        else
        {
            cout << "[Pending]   ";
        }

        cout << tasks[i].description << "\n";
    }

    cout << "================================\n";
}

// Mark a task as completed
void markTaskCompleted()
{
    if (tasks.empty())
    {
        cout << "\nNo tasks available.\n";
        return;
    }

    viewTasks();

    int taskNumber;

    cout << "\nEnter task number to mark as completed: ";
    cin >> taskNumber;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input!\n";
        return;
    }

    if (taskNumber < 1 || taskNumber > static_cast<int>(tasks.size()))
    {
        cout << "Invalid task number!\n";
        return;
    }

    tasks[taskNumber - 1].completed = true;

    cout << "Task marked as completed!\n";
}

// Remove a task
void removeTask()
{
    if (tasks.empty())
    {
        cout << "\nNo tasks available.\n";
        return;
    }

    viewTasks();

    int taskNumber;

    cout << "\nEnter task number to remove: ";
    cin >> taskNumber;

    if (cin.fail())
    {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input!\n";
        return;
    }

    if (taskNumber < 1 || taskNumber > static_cast<int>(tasks.size()))
    {
        cout << "Invalid task number!\n";
        return;
    }

    tasks.erase(tasks.begin() + (taskNumber - 1));

    cout << "Task removed successfully!\n";
}

int main()
{
    int choice;

    cout << "====================================\n";
    cout << "       TO-DO LIST MANAGER\n";
    cout << "====================================\n";

    while (true)
    {
        cout << "\n";
        cout << "1. Add Task\n";
        cout << "2. View Tasks\n";
        cout << "3. Mark Task as Completed\n";
        cout << "4. Remove Task\n";
        cout << "5. Exit\n";
        cout << "------------------------------------\n";
        cout << "Enter your choice: ";

        cin >> choice;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            cout << "Invalid choice! Please enter a number.\n";
            continue;
        }

        switch (choice)
        {
            case 1:
                addTask();
                break;

            case 2:
                viewTasks();
                break;

            case 3:
                markTaskCompleted();
                break;

            case 4:
                removeTask();
                break;

            case 5:
                cout << "\nThank you for using the To-Do List Manager!\n";
                return 0;

            default:
                cout << "Invalid choice! Please select 1-5.\n";
        }
    }

    return 0;
}