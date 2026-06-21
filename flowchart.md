```mermaid
---
config:
  themeVariables:
    primaryTextColor: '#ffffff'
---
flowchart TD
    style Start fill:#00bb00
    
    Start([Program Start]) --> Init["Initialize I2C, Display,<br/>UART, Timer & Interrupts"]
    Init --> Test["Clear Display & Send<br/>Test Message"]
    Test --> CheckRx
    
    subgraph Loop["Main Loop"]
        CheckRx{UART Data<br/>Received?}
        
        %% Force longer vertical links to ensure top entry for next checks
        CheckRx ---->|Yes| ProcessInput
        CheckRx ---->|No| CheckTime
        
        ProcessInput["Parse Input as HH:MM:SS<br/>& Update Timer Value"]
        ProcessInput --> UpdateDisplay
        
        UpdateDisplay["Clear & Refresh Display"]
        UpdateDisplay --> CheckTime
        
        CheckTime{Timer Flag Set<br/>& Timer Active?}
        
        CheckTime ---->|Yes| CheckWrap
        CheckTime ---->|No| CheckButton
        
        CheckWrap{Time >= 24h?}
        
        CheckWrap ---->|Yes| WrapTime
        CheckWrap ---->|No| DisplayTime
        
        WrapTime["Reset to 0"]
        WrapTime --> DisplayTime
        
        DisplayTime["Display & Print Time<br/>Increment Second"]
        DisplayTime --> CheckButton
        
        CheckButton{Button<br/>Pressed?}
        
        CheckButton ---->|Yes| ResetPrompt
        CheckButton ---->|No| CheckRx
        
        ResetPrompt["Reset Timer & Display<br/>Prompt for New Time"]
        ResetPrompt --> CheckRx
    end
    
    subgraph Interrupts["Interrupts"]
        USART["USART RX Interrupt"]
        Button["Button Interrupt"]
        Timer_ISR["Timer Interrupt<br/>1ms Tick"]
    end
    
    USART -.-> SetRxFlag["Set receiveFlag"]
    Button -.-> SetBtnFlag["Set button4Flag"]
    Timer_ISR -.-> SetTimeFlag["Count to 1000ms,<br/>then set itsTime Flag"]
    
    SetRxFlag -.-> CheckRx
    SetBtnFlag -.-> CheckButton
    SetTimeFlag -.-> CheckTime