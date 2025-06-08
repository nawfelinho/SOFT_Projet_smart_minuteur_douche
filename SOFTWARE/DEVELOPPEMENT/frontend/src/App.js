import { Routes, Route } from "react-router-dom";
import UserList from "./components/UserList";
import UserDouches from "./components/UserDouches";
import UserPrevisions from "./components/UserPrevisions";

function App() {
    return (
        <Routes>
            <Route path="/" element={<UserList />} />
            <Route path="/user/:id" element={<UserDouches />} />
            <Route path="/user/:id/previsions" element={<UserPrevisions />} />
        </Routes>
    );
}
export default App;
