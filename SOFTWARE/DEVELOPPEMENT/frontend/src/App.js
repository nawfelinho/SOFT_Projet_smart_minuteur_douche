import { Routes, Route } from "react-router-dom";
import UserList from "./components/UserList";
import UserDouches from "./components/UserDouches";

function App() {
    return (
        <Routes>
            <Route path="/" element={<UserList />} />
            <Route path="/user/:id" element={<UserDouches />} />
        </Routes>
    );
}

export default App;
